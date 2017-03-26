// 
// 
// 

#include "PushButtonManager.h"
#include "Configuration.h"

PushButtonManager::PushButtonManager(int pin, IPushButtonActionsPtr_t pushButtonActions)
	: _pin(pin), _pushButtonActions(pushButtonActions)
{
	pinMode(pin, INPUT);
}

void PushButtonManager::Loop()
{
	auto currentState = digitalRead(_pin);
	if (currentState == ButtonPressed && _previousButtonState == ButtonReleased) //Trigger
	{
		_pressStartTime = millis();
	}

	//handle detection callbacks
	if (currentState == ButtonPressed && _previousButtonState == ButtonPressed) //continue press
	{
		auto length = millis() - _pressStartTime;
		if (!_bLongDetection && _pushButtonActions->GetLongPressPeriod() < length && length < _pushButtonActions->GetVeryLongPressPeriod())
		{
			_bLongDetection = true;
			_pushButtonActions->OnLongPressDetected();
		}
		else if (!_bVeryLongDetection && length > _pushButtonActions->GetVeryLongPressPeriod())
		{
			_bVeryLongDetection = true;
			_pushButtonActions->OnVeryLongPressDetected();
		}
	}

	//Handle button press
	if (currentState == ButtonReleased && _previousButtonState == ButtonPressed && _pressStartTime != 0 && millis() - _pressStartTime > 100) //long enough
	{
		auto length = millis() - _pressStartTime;
		if (length < _pushButtonActions->GetLongPressPeriod()) //less then 5 seconds, regular press
		{
			_pushButtonActions->OnPress(); //Notify change
		}
		else if (length < _pushButtonActions->GetVeryLongPressPeriod()) //less the 20 seconds
		{
			_pushButtonActions->OnLongPress();
		}
		else //over 20 seconds
		{
			_pushButtonActions->OnVeryLongPress();
		}
		//reset all
		_pressStartTime = 0;
		_bLongDetection = false;
		_bVeryLongDetection = false;
	}
	_previousButtonState = currentState;
}
