// CommunicationManager.h

#ifndef _COMMUNICATIONMANAGER_h
#define _COMMUNICATIONMANAGER_h

class ICommand 
{
public:
	virtual void OnCommand(const String &command) = 0;
	virtual ~ICommand() = default;
};
typedef ICommand* ICommandP_t;

template<int PIN>
class CommunicationPin
{
private:
	int _oldState = HIGH;
	const String _command;

public:
	CommunicationPin(const String &command) : _command(command) { pinMode(PIN, INPUT_PULLUP); }

	const String& Command() const { return _command; }

	bool IsTriggered()
	{
		int state = digitalRead(PIN);
		if (state != _oldState)
		{
			Serial.println("Triggered");
			_oldState = state;
			if (state == LOW)
				return true; //triggered
		}
		return false;
	}
};

class CommunicationManager 
{
private:
	ICommandP_t _pCommandSubscriber;
	void OnCommand(const String& command) const;
	CommunicationPin<OpenCommand> _open = CommunicationPin<OpenCommand>("open");
	CommunicationPin<CloseCommand> _close = CommunicationPin<CloseCommand>("close");
	CommunicationPin<StopCommand> _stop = CommunicationPin<StopCommand>("stop");
	CommunicationPin<ButtonPressedCommand> _buttonPressed = CommunicationPin<ButtonPressedCommand>("buttonPressed");

public:
	void Loop();
	CommunicationManager(ICommandP_t commandSubscriber) : _pCommandSubscriber(commandSubscriber) {}
};

typedef CommunicationManager * CommunicationManagerP_t;

#endif

