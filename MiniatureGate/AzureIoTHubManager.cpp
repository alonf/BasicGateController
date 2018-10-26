#include "AzureIoTHubManager.h"
#include "AzureIoTHubHttpClient.h"
#include <ctime>
#include <utility>
#include <Arduino.h>

using namespace std;


AzureIoTHubManager::AzureIoTHubManager(const WiFiManagerPtr_t& wifiManager, LoggerPtr_t logger, const char* connectionString) :  
_logger(std::move(logger)),  _azureIoTHubDeviceConnectionString(connectionString)
{
	wifiManager->RegisterClient([this](ConnectionStatus status) { UpdateStatus(status); });
}

void AzureIoTHubManager::HandleCommand(const String & commandName) const
{
	 auto command = commandName;
	 command.toLowerCase();
	

	_logger->WriteMessage("Received command: ");
	_logger->WriteMessage(command);
	
	if (command != "open" && command != "close" && command != "stop" && command != "web")
	{
		_logger->WriteMessage("Invalid command, should be one of: open, close, stop or web");
		return;
	}

	_pubsub.NotifyAll(command);
}


bool AzureIoTHubManager::CheckTimeInitiated()
{
	if ( _isTimeInitiated)
		return true;
	
	if (_IsInitTime == false)
		return false;

    const time_t epochTime = time(nullptr);

	if (epochTime == 0)
	{
		_logger->WriteErrorMessage("Fetching NTP epoch time failed!", 5);
		delay(100);
		return false;
	}
	//else
	
	_logger->WriteMessage("Fetched NTP epoch time is: ");
	_logger->WriteMessage(epochTime);
	_isTimeInitiated = true;
	return true;
}

bool AzureIoTHubManager::CheckIoTHubClientInitiated()
{
	if (_isIotHubClientInitiated)
		return true;

	if (!AzureIoTHubInit(_azureIoTHubDeviceConnectionString))
		return false;
	_isIotHubClientInitiated = true;
	return true;
}



void AzureIoTHubManager::Loop()
{
	if (CheckTimeInitiated() == false) //can't do anything before setting the time
		return;
	if (CheckIoTHubClientInitiated() == false)
		return;


	/*if ((millis() - _loopStartTime) < 3000)
		return;
	_loopStartTime = millis();*/
	AzureIoTHubLoop();
}

void AzureIoTHubManager::UpdateGateStatus( const char *deviceId, const String& status) const
{
	if (_isIotHubClientInitiated)
		AzureIoTHubSendMessage(deviceId, status.c_str());
}

void AzureIoTHubManager::UpdateStatus(const ConnectionStatus& status)
{
	if (status.IsJustConnected() && !_IsInitTime) //new connection, only once
	{
		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		_IsInitTime = true;
	}
}

