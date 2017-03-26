// WiFiManager.h

#ifndef _WIFIMANAGER_h
#define _WIFIMANAGER_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include <list>

enum class WiFiStatus
{
	connected = 0,			// assigned when connected to a WiFi network;
	noShield = 1,			// assigned when no WiFi shield is present;
	idle = 2,				// it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
	noSSID = 3,				// assigned when no SSID are available;
	scanCompleted = 4,		// assigned when the scan networks is completed;
	connectFailed = 5,		// assigned when the connection fails for all the attempts;
	connectionLost = 6,		// assigned when the connection is lost;
	dissconnected = 7,		// assigned when disconnected from a network;
};

struct AccessPointInfo
{
	String SSID;
	int RSSI;
	bool isEncripted;
};

class ConnectionStatus
{
	friend class WiFiManager;
private:
	const int _status;
	const IPAddress _localIP;
	bool _justConnected;
	bool _justDisconnected;
	bool _isAccessPointMode;

	static std::map<int, WiFiStatus> _statusMap;
	static std::array<String, 8> _messageMap;
	static std::list<AccessPointInfo>   _accessPointList;

	static void ClearAccessPointList() { _accessPointList.clear(); }
	template<typename T>
	static void AddAccessPointInfo(T &&apInfo) { _accessPointList.push_back(std::forward<T>(apInfo)); }

	ConnectionStatus(int status, IPAddress localIP, bool justConnected = false, bool justDisconnected = false, bool isAccessPointMode = false) : _status(status), _localIP(localIP), _justConnected(justConnected), _justDisconnected(justDisconnected), _isAccessPointMode(isAccessPointMode)
	{}

public:
	int WifiCode() const { return _status; }
	WiFiStatus Code() const { return _statusMap[_status]; }
	const String &Message() const {return _messageMap[_status];}
	IPAddress LocalIP() const { return _localIP; }
	bool IsJustConnected() const { return _justConnected; }
	bool IsJustDissconnected() const { return _justDisconnected; }
	bool IsConnected() const { return _status == WL_CONNECTED; }
	bool IsAccessPointModeOn() const { return _isAccessPointMode; }
	static const std::list<AccessPointInfo> &GetAccessPoints() { return _accessPointList; }
};

typedef std::function<void (ConnectionStatus)> wifiNotificarionFunc_t;

class WiFiManager : public Singleton<WiFiManager>, public IProcessor
{
	friend class Singleton<WiFiManager>;
private:

	std::vector<wifiNotificarionFunc_t> _subscribers;
	int _lastConnectionStatus;
	bool _accessPointMode = false;
	bool _accessPointModeHasBeenInit = false;
	bool _initiated = false;

	void UpdateStatus();
	void NotifyAll(ConnectionStatus status) const;
	static void PopulateWiFiNetworks();
	WiFiManager(const String &ssid, const String &password, bool isAccesspointMode);

 public:
	void RegisterClient(wifiNotificarionFunc_t notification);
	bool IsConnected() const;
	void Loop() override;
	void HandleAccessPointModeStatus();
};

typedef std::shared_ptr<WiFiManager> WiFiManagerPtr_t;
#endif

