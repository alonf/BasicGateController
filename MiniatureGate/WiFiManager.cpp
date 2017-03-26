
#include "WiFiManager.h"

using namespace std;

std::map<int, WiFiStatus>  ConnectionStatus::_statusMap =
{
	{ WL_CONNECTED, WiFiStatus::connected },
	{ WL_NO_SHIELD, WiFiStatus::noShield },
	{ WL_IDLE_STATUS, WiFiStatus::idle },
	{ WL_NO_SSID_AVAIL, WiFiStatus::noSSID },
	{ WL_SCAN_COMPLETED, WiFiStatus::scanCompleted },
	{ WL_CONNECT_FAILED, WiFiStatus::connectFailed },
	{ WL_CONNECTION_LOST, WiFiStatus::connectionLost },
	{ WL_DISCONNECTED, WiFiStatus::dissconnected }
};

std::array<String, 8>  ConnectionStatus::_messageMap =
{
	"Connected to a WiFi network",
	"No WiFi shield is present",
	"WiFi.begin(): Trying to connect to a WiFi network",
	"No SSID are available",
	"Scan networks is completed",
	"The connection fails for all the attempts",
	"The connection is lost",
	"Disconnected from a network"
};
std::list<AccessPointInfo>  ConnectionStatus::_accessPointList;

void WiFiManager::PopulateWiFiNetworks()
{
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0)
		Serial.println("no networks found");
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			ConnectionStatus::AddAccessPointInfo(AccessPointInfo{ String(WiFi.ESP8266WiFiScanClass::SSID(i).c_str()), WiFi.ESP8266WiFiScanClass::RSSI(i) ,WiFi.ESP8266WiFiScanClass::encryptionType(i) == ENC_TYPE_NONE });
		}
	}
}

WiFiManager::WiFiManager(const String &ssid, const String &password, bool isAccessPointMode)
{
	PopulateWiFiNetworks();
	if (isAccessPointMode)
	{
		WiFi.mode(WIFI_AP);
		WiFi.disconnect();
		delay(100);

		Serial.print("Setting access point mode to ap name: ");
		Serial.print(ssid.c_str());
		Serial.print("  ap password: ");
		Serial.println(password.c_str());

		WiFi.softAP(ssid.c_str(), password.c_str(), 6);
		Serial.println("softap up and running");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		_accessPointMode = true;
	}
	else
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid.c_str(), password.c_str());
		_lastConnectionStatus = WiFi.status();
	}
}

void  WiFiManager::RegisterClient(wifiNotificarionFunc_t notification)
{
	_subscribers.push_back(notification);
}

void  WiFiManager::NotifyAll(ConnectionStatus status) const
{
	for (auto subscriber : _subscribers)
	{
		subscriber(status);
	}
}

bool WiFiManager::IsConnected() const
{
	return _accessPointMode || WiFi.status() == WL_CONNECTED;
}

void WiFiManager::Loop()
{
	UpdateStatus();
}

void WiFiManager::HandleAccessPointModeStatus()
{
	if (!_accessPointModeHasBeenInit) //first time init access point mode
	{
		_accessPointModeHasBeenInit = true;
		NotifyAll(ConnectionStatus(WiFi.status(), WiFi.localIP(), true, false, true));
		_lastConnectionStatus = 1;
		return;
	}

	auto currentStatus = WiFi.softAPgetStationNum(); //the number of connected client
	if (_lastConnectionStatus == currentStatus) //no change
		return;

	bool justConnected = _lastConnectionStatus == 0 &&  currentStatus > 0; //more than zero
	bool justDissconnected = _lastConnectionStatus > 0 && currentStatus == 0; //zero

	_lastConnectionStatus = currentStatus;
	NotifyAll(ConnectionStatus(WiFi.status(), WiFi.localIP(), justConnected, justDissconnected, true));
}

void WiFiManager::UpdateStatus()
{
	if (_accessPointMode)
	{
		HandleAccessPointModeStatus();
		return;
	}//else
		
	auto currentStatus = WiFi.status();
	if (_lastConnectionStatus == currentStatus) //no change
		return;

	bool justConnected = _lastConnectionStatus != WL_CONNECTED &&  currentStatus == WL_CONNECTED;
	bool justDissconnected = _lastConnectionStatus == WL_CONNECTED && currentStatus != WL_CONNECTED;
	
	_lastConnectionStatus = currentStatus;
	NotifyAll(ConnectionStatus(WiFi.status(), WiFi.localIP(), justConnected, justDissconnected));
}

