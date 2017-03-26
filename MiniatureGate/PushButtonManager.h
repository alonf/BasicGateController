// PushButtonManager.h

#ifndef _PUSHBUTTONMANAGER_h
#define _PUSHBUTTONMANAGER_h

#include "arduino.h"
#include <functional>
#include <memory>
#include "Singleton.h"
#include "ArduinoLoopManager.h"

class IPushButtonActions
{
public:
	virtual void OnPress() = 0; //called when the user pressed and stopped the button for short period
	virtual int GetLongPressPeriod() = 0; //set the time that will be considered as a long press
	virtual void OnLongPressDetected() = 0; //called when the user presses for long time and continues to hold the button
	virtual void OnLongPress() = 0; //called when the user pressed and stopped the button for long period
	virtual int GetVeryLongPressPeriod() = 0; //called when the user presses for very long time and continues to hold
	virtual void OnVeryLongPressDetected() = 0; //called when the user presses for very long time and continues to hold the button
	virtual void OnVeryLongPress() = 0; //called when the user pressed and stopped the button for very long period

	virtual ~IPushButtonActions() {}
};

typedef std::shared_ptr<IPushButtonActions> IPushButtonActionsPtr_t;

class PushButtonManager : public Singleton<PushButtonManager>,  public IProcessor
{
	friend class Singleton<PushButtonManager>;
private:
	const int _pin;
	int _previousButtonState = LOW;
	int _pressStartTime = 0;
	IPushButtonActionsPtr_t _pushButtonActions;
	bool _bLongDetection = false;
	bool _bVeryLongDetection = false;
	PushButtonManager(int pin, IPushButtonActionsPtr_t pushButtonActions);

public:
	virtual ~PushButtonManager()
	{
	}
	void Loop() override;
};

typedef std::shared_ptr<PushButtonManager> PushButtonManagerPtr_t;


#endif

