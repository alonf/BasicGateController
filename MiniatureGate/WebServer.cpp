#include "WebServer.h"
#include "Util.h"
#include "WebSettings.h"
#include <algorithm>


using namespace std;
///*static*/ char WebServer::_setupHtmlBuffer[3072]; //for setup html result
WebServer::WebServer(WiFiManagerPtr_t wifiManager, int port, const char *appKey, std::unique_ptr<DeviceSettings> deviceSettings, std::function<String ()> gateStatusUpdater) :
	_deviceSettings(move(deviceSettings)),
	_server(port), 
	_authorizedUrl(String("/") + appKey),
	_gateStatusUpdater(gateStatusUpdater)
{
	_server.on("/", [this]() { HandleError(); });
	_server.on((_authorizedUrl + "/view.css").c_str(), [this]() { HandleSendViewCSS(); });
	_server.on((_authorizedUrl + "/ap_script.js").c_str(), [this]() { HandleSendAPScript(); });
	_server.on((_authorizedUrl + "/aplist.html").c_str(), [this]() { HandleSendAPList(); });
	_server.on((_authorizedUrl + "/setup").c_str(), [this]() { HandleSetup(); });
	_server.on((_authorizedUrl + "/setconfiguration").c_str(),HTTP_POST, [this]() { HandleSetConfiguration(); });
	_server.on((_authorizedUrl + "/resetaccesspoint").c_str(), [this]() { HandleResetAccessPoint(); });
	_server.on(_authorizedUrl.c_str(), [this]() { HandleMain(); });
	_server.on((_authorizedUrl + "/").c_str(), [this]() { HandleMain(); });
	_server.onNotFound([this]() { HandleError(); });

	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
}

void WebServer::RegisterCommand(WebCommandPtr_t command)
{
	_webCommands.push_back(command);
	auto url = String(CreateUrl(command->TriggerUrl()));
	_server.on(url.c_str(), [=]() {  HandleCommand(command); });
}

void WebServer::SendBackHtml(const String &message)
{
	auto html = String("<html><body><h2>") + _header + "</h2>"
		+ message + "</body></html>";
	_server.send(200,
		"text/html",
		html.c_str());
}
void WebServer::HandleError()
{
	_server.send(401,
		"text/plain",
		"Unauthorized");
}

void WebServer::HandleMain() 
{
	auto html = String("<p><h3>The current gate status is ") +
		(_gateStatusUpdater ? _gateStatusUpdater() : String("not implemented")) + "</h3></p>";
	for (auto webCommand : _webCommands)
	{
		html += String(R"(<p><a href=")") + CreateUrl(webCommand->TriggerUrl()) + String(R"(">)") + webCommand->MenuEntry() + String("</a></p>");
	}
	html += String(R"(<br/><p><a href=")") + CreateUrl("resetaccesspoint") + String(R"(">Factory Reset!</a></p>)");
	SendBackHtml(html);
}

void WebServer::ProcessHTTPSetupRequest()
{
	bool result = PopulateHTMLSetupFromTemplate(WebSettingHtmlTemplate, _templateValuesMap);
	if (result) //finished
	{
		_isHttpSetupRequestOn = false;
		_server.send_P(200, "text/html", _setupHtmlBuffer);
	}
}

void WebServer::HandleSendAPScript()
{
	_server.send_P(200, "text/javascript", APScript);
}

void WebServer::HandleSetup()
{
	_templateValuesMap.clear();
	_templateValuesMap["WFPwd"] = _deviceSettings->accessPointPassword;
	_templateValuesMap["DeviceId"] = _deviceSettings->AzureIoTDeviceId;
	_templateValuesMap["IoTConStr"] = _deviceSettings->azureIoTHubConnectionString;;
	_templateValuesMap["PBLng"] = String(_deviceSettings->longButtonPeriod).c_str();
	_templateValuesMap["PBVLng"] = String(_deviceSettings->veryLongButtonPeriod).c_str();

	const String checked = R"(checked="checked")";
	if (_deviceSettings->shouldUseAzureIoT)
	{
		_templateValuesMap["IoT"] = checked;
		_templateValuesMap["WebSrv"] = "";
	}
	else
	{
		_templateValuesMap["WebSrv"] = checked;
		_templateValuesMap["IoT"] = "";
	}

	_isHttpSetupRequestOn = true; //start request processing
	ProcessHTTPSetupRequest();
}

void WebServer::HandleSendViewCSS()
{
	_server.send_P(200, "text/css", ViewCSS);
}

void WebServer::HandleSendAPList()
{
	auto AccessPointList = ConnectionStatus::GetAccessPoints();
	String html;
	for (auto &&ap : AccessPointList)
	{

		html += R"(<input type = "radio" class="element radio" name = "ap" value = ")";
		html += ap.SSID.c_str();
		html += R"(" )";
		if (ap.SSID == _deviceSettings->ssidName)
			html += R"( checked="checked" )";
		html += R"("/> <label class="choice">)";
		html += ap.SSID.c_str();
		html += R"( <font color="purple">  Signal:)";
		html += String(100 + ap.RSSI).c_str();
		if (!ap.isEncripted)
			html += " Secure";
		html += "</font></label>";

	}
	_server.send(200, "text/html", html);
}

//extern void pp_soft_wdt_stop();
//extern void pp_soft_wdt_restart();

bool WebServer::PopulateHTMLSetupFromTemplate(const String &htmlTemplate, const Util::StringMap & map) 
{
	int startTime = millis();
	do
	{
		if (millis() - startTime > 25) //0.025 seconds per parsing iteration
			return false;
		Serial.printf("Continue setup template processing, index: %d\n", _templateIndex);
		int beginVariable = htmlTemplate.indexOf('%', _templateIndex); //search <%= by searching %
		int endVariable = -1;
		if (beginVariable >= 0) //only if beginVariable didn't reach the end of html
			endVariable = htmlTemplate.indexOf('%', beginVariable + 1);

		if (beginVariable < 0 || endVariable < 0) //no more variables
		{
			auto rest = htmlTemplate.substring(_templateIndex); //add the template end
			memcpy(_setupHtmlBuffer + _templateBufferIndex, rest.c_str(), rest.length()); //copy the template tail
			_templateBufferIndex += rest.length();
			break;
		}

		if (htmlTemplate[beginVariable - 1] != '<' || htmlTemplate[beginVariable + 1] != '=' || htmlTemplate[endVariable + 1] != '>') //not <%= ... %>
		{
			_setupHtmlBuffer[_templateBufferIndex++] = htmlTemplate[_templateIndex];
			++_templateIndex;
			continue;
		}
		auto variableName = htmlTemplate.substring(beginVariable + 2, endVariable);
		String replacedValue = map.at(variableName); //extract only the variable name and replace it
		String htmlUntilVariable = htmlTemplate.substring(_templateIndex, beginVariable - 1);

		//Add all text before the variable and the replacement
		memcpy(_setupHtmlBuffer + _templateBufferIndex, htmlUntilVariable.c_str(), htmlUntilVariable.length());
		_templateBufferIndex += htmlUntilVariable.length();
		memcpy(_setupHtmlBuffer + _templateBufferIndex, replacedValue.c_str(), replacedValue.length());
		_templateBufferIndex += replacedValue.length();
		_setupHtmlBuffer[_templateBufferIndex] = 0;
		_templateIndex = endVariable + 2;
	} while (_templateIndex != _templateEnd);

	//reset for nect time
	_templateIndex = 0;
	_templateBufferIndex = 0;
	_templateEnd = -1;
	return true;
}

void WebServer::HandleSetConfiguration()
{
	_deviceSettings->ssidName = _server.arg("ap").c_str();
	_deviceSettings->accessPointPassword = _server.arg("WFPwd").c_str();
	_deviceSettings->AzureIoTDeviceId = _server.arg("DeviceId").c_str();
	_deviceSettings->azureIoTHubConnectionString = _server.arg("IoTConStr").c_str();
	_deviceSettings->longButtonPeriod = atoi(_server.arg("PBLng").c_str());
	_deviceSettings->veryLongButtonPeriod = atoi(_server.arg("PBVLng").c_str());
	_deviceSettings->shouldUseAzureIoT = _server.arg("WebOrIoT") == "IoT";
	printf("Server arguments:\n");
	for (int i = 0; i < _server.args(); ++i)
	{
		Serial.printf("%s=%s\n", _server.argName(i).c_str(), _server.arg(_server.argName(i)).c_str());
	}

	String html =
		R"(<p><center><h3>The device will reboot and try to connect to:</h3></center></p><p>)";
		html += _deviceSettings->ssidName;
		html += "</p><br/>";
		html += "If after the reboot the two Leds are blinking or the green led is not turned on, do a factory reset by pressing the button for more than ";
		html += String(_deviceSettings->veryLongButtonPeriod / 1000).c_str();
		html += " seconds. The two leds should blink very fast.";

		SendBackHtml(html.c_str());
		_configurationUpdater(*_deviceSettings.get());
		Util::software_Reboot();
}

void WebServer::HandleResetAccessPoint()
{
	_deviceSettings->isFactoryReset = true;
	String html =
		R"(<p><h3>Access point credentials has been reset.</h3></p><br/><p>Reset device to activate access point mode.</p><br/>)";
		html += R"(<p>Set new access point SSID information by surfing to )";
		html += String(R"(http://192.168.4.1/)") + CreateUrl("setup").c_str();
		html += "</p>";

	SendBackHtml(html.c_str());
	_configurationUpdater(*_deviceSettings.get()); //this will reset the device
}
void WebServer::HandleCommand(WebCommandPtr_t webCommand)
{
	//auto html = String("<p><h3>") + webCommand->ResultHTML() +String("</h3></p>");
	_pubsub.NotifyAll(webCommand->Name(), webCommand->Id());
	HandleMain();
	//SendBackHtml(html);
}

bool WebServer::IsConnected() const
{
	return WiFi.status() == WL_CONNECTED;
}

void WebServer::Loop()
{
	if (_isHttpSetupRequestOn)
		ProcessHTTPSetupRequest();
	
	_server.handleClient();
}

void WebServer::SetUpdateConfiguration(std::function<void(const DeviceSettings&)> configurationUpdater)
{
	_configurationUpdater = configurationUpdater;
}

void WebServer::UpdateStatus(ConnectionStatus status)
{
	if (!_isInit && status.IsJustConnected()) //new connection, only once
	{
		Serial.println("Web Server begins...");
		_isInit = true;
		MDNS.begin("esp8266");
		_server.begin();
	}
}

String WebServer::CreateUrl(const String& s) const
{
	return _authorizedUrl + "/" + s;
}
