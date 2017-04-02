// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-device-sdk-c-serializer

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <AzureIoTProtocol_HTTP.h>
#include <AzureIoTHubClient.h>
#include <AzureIoTHub.h>
#include <arduino.h>

/* This sample uses the _LL APIs of iothub_client for example purposes.
That does not mean that HTTP only works with the _LL APIs.
Simply changing the using the convenience layer (functions not having _LL)
and removing calls to _DoWork will yield the same results. */

#ifdef ARDUINO
#include "IoTHub.h"
#else
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "serializer.h"
#include "iothub_client_ll.h"
#include "iothubtransporthttp.h"
#endif

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
//static const char* connectionString = "[device connection string]";
static const char* connectionString = "HostName=Codemotion.azure-devices.net;DeviceId=SimpleRelay;SharedAccessKey=2KWShSdHMTL4bXpqqtz3jze8vRq7nEKLvYQdSpQpQWI=";

static int g_relayState = LOW;


void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	unsigned int messageTrackingId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(userContextCallback));

	printf("Message Id: %u Received.\r\n", messageTrackingId);
	printf("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}


/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	IOTHUBMESSAGE_DISPOSITION_RESULT result;
	const unsigned char* buffer = nullptr;
	size_t size = 0;
	
	if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		printf("unable to IoTHubMessage_GetByteArray\r\n");
		return IOTHUBMESSAGE_REJECTED;
	}

	printf("Recived command, size:%d text:", size);
	for (int i = 0; i < size; ++i)
		putchar(buffer[i]);
	putchar('\n');

	/*buffer is not zero terminated*/
	if (strncmp("Toggle", reinterpret_cast<const char *>(buffer), size) == 0)
	{
		g_relayState = g_relayState == LOW ? HIGH : LOW; //Toggle Relay State
		printf("Setting the relay state to: %s\r\n", g_relayState == HIGH ? "On" : "Off");
		digitalWrite(RelayPin, g_relayState);
		return IOTHUBMESSAGE_ACCEPTED;
	}
	return IOTHUBMESSAGE_ABANDONED;
}

void IoTHubLoop()
{
	if (serializer_init(nullptr) != SERIALIZER_OK)
	{
		printf("Failed on serializer_init\r\n");
	}
	else
	{
		IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
			
		srand(static_cast<unsigned int>(time(nullptr)));

		if (iotHubClientHandle == nullptr)
		{
			printf("Failed on IoTHubClient_LL_Create\r\n");
		}
		else
		{
			// Because it can poll "after 9 seconds" polls will happen 
			// effectively at ~10 seconds.
			// Note that for scalabilty, the default value of minimumPollingTime
			// is 25 minutes. For more information, see:
			// https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
			unsigned int minimumPollingTime = 9;
			
			if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
			{
				printf("failure to set option \"MinimumPollingTime\"\r\n");
			}
						
			if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, NULL) != IOTHUB_CLIENT_OK)
			{
				printf("unable to IoTHubClient_SetMessageCallback\r\n");
			}
					/* wait for commands */
			while (1)
			{
				IoTHubClient_LL_DoWork(iotHubClientHandle);
				ThreadAPI_Sleep(10);
			}
			IoTHubClient_LL_Destroy(iotHubClientHandle);
		}
	}
}