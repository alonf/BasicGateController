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
const int OpeningDirection = -1; //set the initial motor direction (-1 or 1)
const int OpenCommand = 2;
const int CloseCommand = 3;
const int StopCommand = 5;
const int ButtonPressedCommand = 8;
#endif

