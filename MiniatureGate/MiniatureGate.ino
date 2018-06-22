#include <WiFiClient.h>
#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_HTTP.h>
#include <AzureIoTProtocol_MQTT.h>
#include <dummy.h>
#include <EEPROM.h>
#include "ConfigurationManager.h"
#include "ArduinoLoopManager.h"
#include "Singleton.h"
#include "Util.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "Logger.h"
#include "AzureIoTHubManager.h"
#include <memory>
#include "Configuration.h"
#include "GateManager.h"
#include "PushButtonManager.h"
#include <Arduino.h>

using namespace std;

void OnButtonPressed();
void Reset();
void ResetToAccessPointMode();
LoggerPtr_t logger;
WiFiManagerPtr_t wifiManager;
GateManagerPtr_t gateManager;
PushButtonManagerPtr_t pushButtonManager;
ArduinoLoopManager_t loopManager;
ConfigurationManager_t configurationManger;

template<typename T>
void SubscribeRemoteCommands(std::shared_ptr<T> server)
{
	server->Register([](const String &commandName) {logger->OnCommand(commandName); });
	server->Register([](const String &commandName) {gateManager->OnCommand(commandName); });
}


AzureIoTHubManagerPtr_t azureIoTHubManager;
static bool g_shouldReset = false;

//handle a command from the cloud that switch to local web server mode
void OnCommand(const String &commandName)
{
	String command = commandName;
	command.toLowerCase();
	if (command == "web")
	{
		configurationManger->SetWebServerMode();
		configurationManger->FlashEEProm();
		g_shouldReset = true; //must finish handling cloud command before reset.

	}
}

void SetupAzureIoTHubManager()
{
	char *connectionString = strdup(configurationManger->GetAzureIoTConnectionString().c_str()); //use only once, so not really a memory leak
	azureIoTHubManager = AzureIoTHubManager::Create(wifiManager, logger, connectionString);
	SubscribeRemoteCommands(azureIoTHubManager);
	azureIoTHubManager->Register([](const String &commandName) {OnCommand(commandName); });
}

class WebCommand : public IWebCommand, public enable_shared_from_this<WebCommand>
{
private:
	const String _menuEnrty;
	const String _commandName;
	const String _resultHtml;
	weak_ptr<WebServer> _webServer;

public:
	WebCommand(const String &menuEntry, const String & commandName, const WebServerPtr_t& webServer) : _menuEnrty(menuEntry), _commandName(commandName),
		_resultHtml(String("Processing ") + _commandName + " Command"), _webServer(webServer)
	{
	}

	void Register()
	{
		_webServer.lock()->RegisterCommand(shared_from_this());
	}

	const String& MenuEntry() const override
	{
		return _menuEnrty;
	}

	const String& Name() const override
	{
		return _commandName;
	}

	const String& ResultHTML() const override
	{
		return _resultHtml;
	}
	const String& TriggerUrl() const override
	{
		return _commandName;
	}
};

WebServerPtr_t webServer;

String gateStatus;


void SetupWebServer()
{
	auto deviceSettings = unique_ptr<DeviceSettings>(new DeviceSettings());
	deviceSettings->isFactoryReset = false;
	deviceSettings->ssidName = configurationManger->GetSSID();
	deviceSettings->accessPointPassword = configurationManger->GetAccessPointPassword();
	deviceSettings->shouldUseAzureIoT = configurationManger->ShouldUseAzureIoTHub();
	deviceSettings->azureIoTHubConnectionString = configurationManger->GetAzureIoTConnectionString();
	deviceSettings->AzureIoTDeviceId = configurationManger->GetIoTHubDeviceId();
	deviceSettings->longButtonPeriod = configurationManger->GetLongPeriodButonPressTimesMilliSeconds();
	deviceSettings->veryLongButtonPeriod = configurationManger->GetVeryLongPeriodButonPressTimesMilliSeconds();

	webServer = WebServer::Create(wifiManager, 80, appKey, std::move(deviceSettings), []() { return gateStatus; });
	webServer->SetWebSiteHeader(String(webSiteHeader));
	webServer->SetUpdateConfiguration([](const DeviceSettings& deviceSettings)
	{
		if (deviceSettings.isFactoryReset)
		{
			configurationManger->FacrotyReset();
			return; //never reach this line, the device should reset itself
		}

		configurationManger->SetWiFiCredentials(deviceSettings.ssidName, deviceSettings.accessPointPassword);
		if (deviceSettings.shouldUseAzureIoT)
		{
			Serial.println("deviceSettings.shouldUseAzureIoT");
			configurationManger->SetAzureIoTHubInformation(deviceSettings.azureIoTHubConnectionString, deviceSettings.AzureIoTDeviceId);
		}
		else
		{
			configurationManger->SetWebServerMode();
		}
		configurationManger->SetButonPressTimesMilliSeconds(deviceSettings.longButtonPeriod, deviceSettings.veryLongButtonPeriod);
		configurationManger->FlashEEProm();
	});

	SubscribeRemoteCommands(webServer);
	make_shared<WebCommand>(openMenuEntry, "Open", webServer)->Register();
	make_shared<WebCommand>(stopMenuEntry, "Stop", webServer)->Register();
	make_shared<WebCommand>(closeMenuEntry, "Close", webServer)->Register();

}



class PushButtonActions final : public IPushButtonActions
{
private:
	void OnPress() override { OnButtonPressed(); }
	unsigned int GetLongPressPeriod() override { return 5000; } //5 seconds
	void OnLongPressDetected() override { logger->OnLongButtonPressDetection(); }
	void OnLongPress() override { Reset(); }
	unsigned int GetVeryLongPressPeriod() override { return 20000; }//20 seconds
	void OnVeryLongPressDetected() override { logger->OnVeryLongButtonPressDetection(); }
	void OnVeryLongPress() override { ResetToAccessPointMode(); }
};

void setup()
{
	configurationManger = ConfigurationManager::Create();

	auto storedSSID = configurationManger->GetSSID();
	auto storedPassword = configurationManger->GetAccessPointPassword();

	logger = Logger::Create(redLed, greenLed, 115200);

	if (!configurationManger->IsAccessPointMode())
	{
		Serial.println("Try to connect to WiFi Access Point");
		Serial.print("Stored SSID is:");
		Serial.println(storedSSID.c_str());

		wifiManager = WiFiManager::Create(storedSSID, storedPassword, false);
	}
	else //Set access point mode
	{
		Serial.println("Create Access Point Mode");
		Serial.print("Stored SSID is:");
		Serial.println(storedSSID.c_str());

		wifiManager = WiFiManager::Create(SSID, password, true);
	}

	wifiManager->RegisterClient([](const ConnectionStatus &status) { logger->OnWiFiStatusChanged(status); });

	if (configurationManger->ShouldUseAzureIoTHub())
	{
		SetupAzureIoTHubManager();
	}
	else
	{
		SetupWebServer();
	}

	gateManager = GateManager::Create([=](const String &gateStatus)
	{
		logger->WriteMessage(gateStatus);
		if (configurationManger->ShouldUseAzureIoTHub())
		{
			azureIoTHubManager->UpdateGateStatus(configurationManger->GetIoTHubDeviceId().c_str(), gateStatus); 
		}
		::gateStatus = gateStatus;
	});

	pushButtonManager = PushButtonManager::Create(pushButton, make_shared<PushButtonActions>());

	if (configurationManger->ShouldUseAzureIoTHub())
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{ logger, wifiManager, pushButtonManager, gateManager, azureIoTHubManager });
	}
	else
	{
		loopManager = ArduinoLoopManager::Create(initializer_list<processor_t>{logger, wifiManager, pushButtonManager, gateManager, webServer });
	}
	logger->TestLeds();
	configurationManger->DumpEEPromInfo();
}



void loop()
{
	if (g_shouldReset)
		Reset();

	loopManager->Loop();
	
}

void OnButtonPressed()
{
	gateManager->OnButtonPressed();
}

void ResetToAccessPointMode()
{
	configurationManger->FacrotyReset(); //switch to access point mode and reset the device
}

void Reset()
{
	Util::software_Reboot();
}
