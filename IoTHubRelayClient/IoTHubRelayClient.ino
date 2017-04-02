#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_HTTP.h>
#include <AzureIoTHubClient.h>
#include <AzureIoTHub.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "IoTHub.h"

//static char ssid[] = "Lumia 950 XL Alon";     // your network SSID (name)
static char ssid[] = "DellXPSAlon";     // your network SSID (name)
static char pass[] = "AF123456";    // your network password (use for WPA, or use as key for WEP)

static WiFiClientSecure sslClient; // for ESP8266

static AzureIoTHubClient iotHubClient;

void initSerial();
void initWifi();
void initTime();

void setup() 
{
	initSerial();
	initWifi();
	initTime();
	pinMode(RelayPin, OUTPUT);
	iotHubClient.begin(sslClient);
}

void loop() 
{
	IoTHubLoop();
}

void initSerial() 
{
	// Start serial and initialize stdout
	Serial.begin(115200);
	Serial.setDebugOutput(true);
}

void initWifi() 
{
	// check for the presence of the shield :
	if (WiFi.status() == WL_NO_SHIELD) 
	{
		Serial.println("WiFi shield not present");
		// don't continue:
		while (true)
			;
	}

	// attempt to connect to Wifi network:
	Serial.print("Attempting to connect to SSID: ");
	Serial.println(ssid);

	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	WiFi.begin(ssid, pass);
	while (WiFi.status() != WL_CONNECTED) 
	{
		// unsuccessful, retry in 4 seconds
		Serial.print("failed ... ");
		delay(4000);
		Serial.print("retrying ... ");
	}

	Serial.println("Connected to wifi");
}

void initTime() 
{
	time_t epochTime;

	configTime(0, 0, "pool.ntp.org", "time.nist.gov");

	while (true) 
	{
		epochTime = time(NULL);

		if (epochTime == 0) 
		{
			Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
			delay(2000);
		}
		else 
		{
			Serial.print("Fetched NTP epoch time is: ");
			Serial.println(epochTime);
			break;
		}
	}
}

