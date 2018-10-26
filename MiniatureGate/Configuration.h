// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h
#include <Arduino.h>

//Wifi Configuration
static const char *SSID = "AlonIoT"; //The default configuration access point ssid
static const char *password = "12345678"; //The default configuration access point password - at least 8 chars
static const char *webSiteHeader = "Welcome to the Minature Gate Cloud Controller";
static const char *appKey = "appkey"; //your secret app key - should be long enough
static char* deviceId = "Gate";

static const char *stopMenuEntry = "Stop";
static const char *closeMenuEntry = "Close Gate";
static const char *openMenuEntry = "Open Gate";

const unsigned int defaultButtonLongTimePeriod = 5000; //5 seconds -> reset
const unsigned int defaultButtonVeryLongTimePeriod = 20000; //20 seconds -> factory reset

//WeMos D1 Board configuration
const int pushButton = D4; //D4 Pulldup 10K
const int redLed = D6; 
const int greenLed = D7; 
const int ButtonPressed = LOW;
const int ButtonReleased = ButtonPressed == HIGH ? LOW : HIGH;

enum class Command : unsigned char
{
	None = 0b000,
	Open = 0b001,
	Close = 0b010,
	Stop = 0b011,
	Button = 0b100,
};

enum class GateStatus : unsigned char
{
	NoStatus = 0b000,
	OpenedMessage = 0b001,
	OpeningMessage = 0b010,
	StopedMessage = 0b011,
	ClosedMessage = 0b100,
	ClosingMessage = 0b101,
	UnknownMessage = 0b110,
	NoStatus1 = 0b111
};


const int CommunicationLine0 = D2;
const int CommunicationLine1 = D1;
const int CommunicationLine2 = D5;
const int CommunicationLineDirection = D0; //LOW => master send command to slave, HIGH => master request data from slave
const unsigned int StatusPollingInterval = 500; //0.5 seconds

#endif

