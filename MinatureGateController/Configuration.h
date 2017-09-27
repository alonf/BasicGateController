// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h


//Pro micro gate controll with stepper motor & limit switches configuration
const int stepper1 = 16;  //in1=>15, in2=>14, in3=>16,in4=>10
const int stepper2 = 15; 
const int stepper3 = 10;  
const int stepper4 = 14; 
const int limitSwitcheOpened = 6;
const int limitSwitcheClosed = 7;
const int flashingLED = 4; 
const int flashingLedPeriod = 500; //0.5 second
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
const int stepperSpeed = 40; // 0 > speed < 1024
const int OpeningDirection = 1; //set the initial motor direction (-1 or 1)

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
	OpenningMessage = 0b010,
	StopedMessage = 0b011,
	ClosedMessage = 0b100,
	ClosingMessage = 0b101,
	UnknownMessage = 0b110,
	NoStatus1 = 0b111
};



const int CommunicationLine0 = 3;
const int CommunicationLine1 = 2;
const int CommunicationLine2 = 5;
const int CommunicationLineDirection = 8; //0=> master send command to slave, 1 => master request data from slave

#endif

