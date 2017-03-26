// AzureIoTHubManager.h

#ifndef _AZUREIOTHUBMANAGER_h
#define _AZUREIOTHUBMANAGER_h

#include "Arduino.h"
#include "AzureIoTHubHttpClient.h"
#include "AzureIoTHub.h"
#include <functional>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"
#include "Singleton.h"
#include "ArduinoLoopManager.h"

typedef std::function<void(const String&, int)> IAzureIoTHubCommandNotificationPtr_t;
class AzureIoTHubManager : public Singleton<AzureIoTHubManager>, public IProcessor
{
	friend class Singleton<AzureIoTHubManager>;

private:
	static WiFiClientSecure _sslWiFiClient;
	static AzureIoTHubClient _iotHubClient;
	bool _isTimeInitiated = false;
	bool _isIotHubClientInitiated = false;
	PubSub<AzureIoTHubManager, const String&, int> _pubsub;
	bool _relayState = false;
	LoggerPtr_t _logger;
	bool _IsInitTime = false;
	int _loopStartTime = millis();
	const char* _azureIoTHubDeviceConnectionString;
	void UpdateStatus(ConnectionStatus status);
	bool CheckTimeInitiated();
	bool CheckIoTHubClientInitiated();
	AzureIoTHubManager(WiFiManagerPtr_t wifiManager, LoggerPtr_t logger, const char* connectionString);
public:
	void Register(WebNotificationPtr_t subscriber) { _pubsub.Register(subscriber); }
	void Loop() override;
	void UpdateGateStatus(char *deviceId, String status) const;
	void HandleCommand(const String& commandName) const;

};


typedef std::shared_ptr<AzureIoTHubManager> AzureIoTHubManagerPtr_t;
#endif

