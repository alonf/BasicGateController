#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include "AzureIoTHub.h"
#include "AzureIoTHubManager.h"
#include "azure_c_shared_utility\platform.h"

char  const *__ctype_ptr__ = 0;

void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	int messageTrackingId = reinterpret_cast<intptr_t>(userContextCallback);

	printf_P("Message Id: %d Received.\r\n", messageTrackingId);

	printf_P("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}


static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	printf_P("IoTHubMessage\n");
	IOTHUBMESSAGE_DISPOSITION_RESULT result;
	const unsigned char* buffer;
	size_t size;
	if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		printf_P("unable to IoTHubMessage_GetByteArray\r\n");
		return IOTHUBMESSAGE_REJECTED;
	}// else
	char commandBuffer[16]; //command should be less then 15 characters
	
	if (size > sizeof(commandBuffer) - 1) //copy up to 15 chars
		size = sizeof(commandBuffer) - 1;
	
	memcpy(commandBuffer, buffer, size); 
	commandBuffer[size] = 0;
	String command(commandBuffer);
	
	printf_P("Message arrived, command:%s\n", command.c_str());
	AzureIoTHubManager::Instance()->HandleCommand(command);
			
	return IOTHUBMESSAGE_ACCEPTED;
}

IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

extern "C" bool AzureIoTHubInit(const char *connectionString)
{
	Serial.printf("Connection string: %s\n", connectionString);
	if (platform_init() != 0)
	{
		printf_P("Failed init platform.\n");
		return false;
	}
	for (int i = 0; i < 5; ++i) //try to use MQTT 5 times, and then fall to HTTP
	{
		iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
		if (iotHubClientHandle != nullptr)
			break;
	
		printf_P("Failed create with MQTT try #%d\n", i);
	}

	if (iotHubClientHandle == nullptr)
	{
		iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
		if (iotHubClientHandle == nullptr)
		{
			printf_P("Failed create with HTTP\n");
			return false;
		}
		//HTTP requires polling
		unsigned int minimumPollingTime = 9; /*because it can poll "after 9 seconds" polls will happen effectively at 4 seconds*/
		if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
		{
			printf_P("failure in \"MinimumPollingTime\"\n");
			return false;
		}
	}
	
	if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, nullptr) != IOTHUB_CLIENT_OK)
	{
		printf_P("unable to SetMessageCallback\n");
		return false;
	}
	printf_P("IoTHubClient initiated\n");
	return true;
}

extern "C" bool AzureIoTHubSendMessage(const char *deviceId, const char *status)
{
	bool result = false;

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(status);
	if (messageHandle == nullptr)
	{
		printf_P("unable to create a new IoTHubMessage\r\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, reinterpret_cast<void*>(1)) != IOTHUB_CLIENT_OK)
		{
			printf_P("failed to hand over the message to IoTHubClient");
		}
		else
		{
			printf_P("IoTHubClient accepted the message for delivery\r\n");
			result = true;
		}

		IoTHubMessage_Destroy(messageHandle);
	}
	return result;
}

extern "C" void AzureIoTHubLoop(void)
{
	static unsigned int t = 0;
	if (++t % 100000 == 0)
		printf_P(".");
	IoTHubClient_LL_DoWork(iotHubClientHandle);
}



