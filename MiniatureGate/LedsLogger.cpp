
#include "LedsLogger.h"

LedsLogger::Led::Led(int ledPin): _ledPin(ledPin), _delayBeforeStart(0), _currentBlinkingIpDigit(2), _bBlinkingIpAddress(false), _currentBlinkingIpOctec(0)
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, _ledValue);
}

void LedsLogger::Led::Update()
{
	//Handle delay before start
	if (_delayBeforeStart > 0 && millis() - _startTime < _delayBeforeStart)
		return;

	if (_delayBeforeStart > 0) //reset delay before start
	{
		_delayBeforeStart = 0; 
		_startTime = millis();
		return;
	}
	
	//handle time between blinks
	if (_times == 0 || millis() - _startTime < _blinkDelay)
		return;

	_startTime = millis();
	_ledValue = _ledValue == HIGH ? LOW : HIGH;
	digitalWrite(_ledPin, _ledValue);
	--_times;
	if (_times == 0) //end blinking
	{
		if (_bBlinkingIpAddress)
		{
			BlinkNextIpDigit();
		}
		else
		{
			digitalWrite(_ledPin, _lastSetVLedValue); //at the end, set the led to the last value
		}
	}
}

void LedsLogger::Led::BlinkNextIpDigit()
{
	//Serial.println("Print next digit");
	int delayBetweenOctec = 0;
	if (_currentBlinkingIpDigit < 0) //move to next octec
	{
		_currentBlinkingIpDigit = 2;
		_currentBlinkingIpOctec++;
		delayBetweenOctec = 250;
	}

	if (_currentBlinkingIpOctec > 3) //finish?
	{
		_bBlinkingIpAddress = false;
		_currentBlinkingIpDigit = 2;
		_currentBlinkingIpOctec = 0;
		digitalWrite(_ledPin, _lastSetVLedValue); //at the end, set the led to the last value
		Serial.println("Finish blinking IP Address");
		return;
	}

	auto digit = GetDigit(_ipAddress[_currentBlinkingIpOctec], _currentBlinkingIpDigit);
	//Serial.printf("Blinking IP Address, octec #%d, digit #%d, digit: %d\n", _currentBlinkingIpOctec, _currentBlinkingIpDigit, digit);
	
	if (digit == 0)
	{
		DoBlink(1, 100, 500 + delayBetweenOctec); //blink shorter period to indicate 0
	}
	else
	{
		DoBlink(digit, 500, 1000 + delayBetweenOctec);
	}
	delayBetweenOctec = 0;
	--_currentBlinkingIpDigit;
}

int LedsLogger::Led::GetDigit(int from, int index)
{
	return (from / static_cast<int>(pow(10.0, index))) % 10;
}

void LedsLogger::Led::DoBlink(int times, int delay, int delayBeforeStart /*= 0 */)
{
	digitalWrite(_ledPin, LOW);
	_delayBeforeStart = delayBeforeStart;
	_blinkDelay = delay;
	_times = times * 2 + 1; //each one is two: on and off, + one for the last on
	_startTime = millis();
}

void LedsLogger::Led::Blink(int times, int delay, int delayBeforeStart /*= 0 */)
{
	if (_bBlinkingIpAddress) //block other blink operations
		return;
	DoBlink(times, delay, delayBeforeStart);
}

void LedsLogger::Led::Set(int value)
{
	_times = 0; //stop blinking
	_ledValue = _lastSetVLedValue = value;
	digitalWrite(_ledPin, _ledValue);
}

void LedsLogger::Led::BlinkIpAddress(const IPAddress& ipAddress)
{
	if (_bBlinkingIpAddress) //already blinking an IP Address
		return;

	_ipAddress = ipAddress;
	_currentBlinkingIpDigit = 2;
	_currentBlinkingIpOctec = 0;
	_bBlinkingIpAddress = true;
	DoBlink(_ipAddress[0], 50);
}
