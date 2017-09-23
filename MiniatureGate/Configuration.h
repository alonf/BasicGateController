// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

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
const int I2CAddress = 8;
const int OpenCommand = D1;
const int CloseCommand = D2;
const int StopCommand = D5;
const int ButtonPressedCommand = D0;
#endif

