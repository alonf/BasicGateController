#ifndef _LEDSLOGGER_h
#define _LEDSLOGGER_h

#include <memory>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include <IPAddress.h>


class LedsLogger : public Singleton<LedsLogger>, public IProcessor
{
 private:
	 friend class Singleton<LedsLogger>;
	 class Led
	 {
	 private:
		 const int _ledPin;
         unsigned int _ledValue = 0;
         unsigned int _lastSetVLedValue = 0;
         unsigned int _times = 0;
         unsigned int _blinkDelay = 0;
		 unsigned long _startTime = 0;
		 unsigned int _delayBeforeStart;
         int _currentBlinkingIpDigit;
		 IPAddress _ipAddress;
		 bool _bBlinkingIpAddress;
         unsigned int _currentBlinkingIpOctec;
		 static int GetDigit(int from, int index);
		 void DoBlink(unsigned int times, unsigned int delay, unsigned int delayBeforeStart = 0);
		 void BlinkNextIpDigit();
	 public:
		 Led(int ledPin);
		 void Update();
		 void BlinkIpAddress(const IPAddress& ipAddress);
		 void Blink(unsigned int times, unsigned int delay, unsigned int delayBeforeStart = 0);
		 void Set(int value);
	 };

	 Led _red;
	 Led _green;
	 
	 LedsLogger(int redLedPin, int greenLedPin) : _red(redLedPin), _green(greenLedPin) {}

 public:
	 const char *GetName() const override { return "LedsLogger"; }
	 void BlinkRed(unsigned int times, unsigned int delay, unsigned int delayBeforeStart = 0) { _red.Blink(times, delay, delayBeforeStart); }
	 void BlinkGreen(unsigned int times, unsigned int delay, unsigned int delayBeforeStart = 0) { _green.Blink(times, delay, delayBeforeStart); }
	 void SetRed(int value) { _red.Set(value); }
	 void SetGreen(int value) { _green.Set(value); }
	 
	 void Loop() override
	 { //call this function in a tight interval
		 _red.Update();
		 _green.Update();
	 }

	 void BlinkIpAddress(const IPAddress& ipAddress) { _green.BlinkIpAddress(ipAddress); }
};

typedef std::shared_ptr<LedsLogger> LedsLoggerPtr_t;

#endif

