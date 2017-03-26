#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include "AzureIoTHub.h"
#include "AzureIoTHubManager.h"


void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	int messageTrackingId = reinterpret_cast<intptr_t>(userContextCallback);

	LogInfo("Message Id: %d Received.\r\n", messageTrackingId);

	LogInfo("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

//static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
//{
//	static unsigned int messageTrackingId;
//	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
//	if (messageHandle == nullptr)
//	{
//		LogInfo("unable to create a new IoTHubMessage\r\n");
//	}
//	else
//	{
//		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback,
//			reinterpret_cast<void*>(static_cast<uintptr_t>(messageTrackingId))) != IOTHUB_CLIENT_OK)
//		{
//			LogInfo("failed to hand over the message to IoTHubClient");
//		}
//		else
//		{
//			LogInfo("IoTHubClient accepted the message for delivery\r\n");
//		}
//		IoTHubMessage_Destroy(messageHandle);
//	}
//	free(const_cast<unsigned char*>(buffer));
//	messageTrackingId++;
//}

static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	LogInfo("IoTHubMessage\n");
	IOTHUBMESSAGE_DISPOSITION_RESULT result;
	const unsigned char* buffer;
	size_t size;
	if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		LogInfo("unable to IoTHubMessage_GetByteArray\r\n");
		return IOTHUBMESSAGE_REJECTED;
	}// else
	char commandBuffer[16]; //command should be less then 15 characters
	
	if (size > sizeof(commandBuffer) - 1) //copy up to 15 chars
		size = sizeof(commandBuffer) - 1;
	
	memcpy(commandBuffer, buffer, size); 
	commandBuffer[size] = 0;
	String command(commandBuffer);
	
	Serial.printf("Message arrived, command:%s\n", command.c_str());
	AzureIoTHubManager::Instance()->HandleCommand(command);
			
	return IOTHUBMESSAGE_ACCEPTED;
}

IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

extern "C" bool AzureIoTHubInit(const char *connectionString)
{
	Serial.printf("Connection string: %s\n", connectionString);
	
	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
	srand(static_cast<unsigned int>(time(nullptr)));

	if (iotHubClientHandle == nullptr)
	{
		LogInfo("Failed on IoTHubClient_LL_Create\r\n");
		return false;
	}


	unsigned int minimumPollingTime = 3; /*because it can poll "after 9 seconds" polls will happen effectively at 4 seconds*/
	if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
	{
		LogInfo("failure to set option \"MinimumPollingTime\"\r\n");
		return false;
	}

	if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, nullptr) != IOTHUB_CLIENT_OK)
	{
		LogInfo("unable to IoTHubClient_SetMessageCallback\r\n");
		return false;
	}
	LogInfo("IoTHubClient initiated\r\n");
	return true;
}

extern "C" bool AzureIoTHubSendMessage(char *deviceId, const char *status)
{
	bool result = false;

	unsigned char* destination;
	size_t destinationSize;
	

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
	if (messageHandle == nullptr)
	{
		LogInfo("unable to create a new IoTHubMessage\r\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, reinterpret_cast<void*>(1)) != IOTHUB_CLIENT_OK)
		{
			LogInfo("failed to hand over the message to IoTHubClient");
		}
		else
		{
			LogInfo("IoTHubClient accepted the message for delivery\r\n");
			result = true;
		}

		IoTHubMessage_Destroy(messageHandle);
	}
	free(destination);
	return result;
}


extern "C" void AzureIoTHubLoop(void)
{
	Serial.write(".");
	IoTHubClient_LL_DoWork(iotHubClientHandle);
}
