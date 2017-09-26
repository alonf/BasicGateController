#include "GateManager.h"

GateManager::GateManager() : _state(nullptr)
{
	Serial.println("Gate Manager has started");
}


void GateManager::Initialize() //called by the singleton create, after ctor
{
	Serial.println("Initializing...");

	static auto gateMovementStandstillInTheMiddleState = GateMovementStandstillInTheMiddleState<GateState::UNKNOWN>(this);
	_states[static_cast<unsigned char>(GateState::UNKNOWN)] = &gateMovementStandstillInTheMiddleState;
	
	static auto gateMovementOpenedState = GateMovementOpenedClosedState<GateState::OPENED >(this);
	_states[static_cast<unsigned char>(GateState::OPENED)] = &gateMovementOpenedState;

	static auto gateMovementOpennign = GateMovementOpennignOrClosing<OpeningDirection, GateState::OPENNING, GateState::OPENED>(this);
	_states[static_cast<unsigned char>(GateState::OPENNING)] = &gateMovementOpennign;

	static auto gateMovementStopping = GateMovementStopping(this);
	_states[static_cast<unsigned char>(GateState::STOPPING)] = &gateMovementStopping;

	static auto gateMovementStopped = GateMovementStandstillInTheMiddleState<GateState::STOPPED>(this);
	_states[static_cast<unsigned char>(GateState::STOPPED)] = &gateMovementStopped;

	static auto gateMovementClosing = GateMovementOpennignOrClosing<-OpeningDirection, GateState::CLOSING, GateState::CLOSED>(this);
	_states[static_cast<unsigned char>(GateState::CLOSING)] = &gateMovementClosing;

	static auto gateMovementClosedState = GateMovementOpenedClosedState<GateState::CLOSED>(this);
	_states[static_cast<unsigned char>(GateState::CLOSED)] = &gateMovementClosedState;

	
	_statuses[static_cast<unsigned char>(GateState::UNKNOWN)] = "Unknown";
	_statuses[static_cast<unsigned char>(GateState::OPENED)] = "Opened";
	_statuses[static_cast<unsigned char>(GateState::OPENNING)] = "Opening";
	_statuses[static_cast<unsigned char>(GateState::STOPPING)] = "Stopping";
	_statuses[static_cast<unsigned char>(GateState::STOPPED)] = "Stopped";
	_statuses[static_cast<unsigned char>(GateState::CLOSING)] = "Closing";
	_statuses[static_cast<unsigned char>(GateState::CLOSED)] = "Closed";
	Serial.println("Initializing Pins...");
	pinMode(flashingLED, OUTPUT);
	pinMode(limitSwitcheClosed, INPUT_PULLUP);
	pinMode(limitSwitcheOpened, INPUT_PULLUP);
	Serial.println("Initializing, setting state...");
	ChangeState(SelectState(ReadState()));
	Serial.println("Initializing, finished...");
}


void GateManager::ChangeState(GateMovementState *pState)
{
	
	Serial.write("Gate change state. Old state: ");
	Serial.println(_state ? _statuses[static_cast<unsigned char>(_state->State())].c_str() : "Initizlizing");
	_state = pState;	
	auto newStatus = _statuses[static_cast<unsigned char>(_state->State())].c_str();
	Serial.write("New state: ");
	Serial.println(newStatus);
	
	_state->Initialize();
}



GateState GateManager::ReadState()
{
	int opennedLimitSwitchValue = digitalRead(limitSwitcheOpened);
	int closedLimitSwitchValue = digitalRead(limitSwitcheClosed);

	/*Serial.write("limitSwitcheOpened: ");
	Serial.println(opennedLimitSwitchValue);
	Serial.write("limitSwitcheClosed: ");
	Serial.println(closedLimitSwitchValue);*/
	//limit switch log information	
	static int oldOpennedLimitSwitchValue;
	static int oldClosedLimitSwitchValue;

	if (opennedLimitSwitchValue != oldOpennedLimitSwitchValue || closedLimitSwitchValue != oldClosedLimitSwitchValue)
	{
		Serial.write("Openned limit switch value: ");
		Serial.println(opennedLimitSwitchValue);

		Serial.write("Closed limit switch value: ");
		Serial.println(closedLimitSwitchValue);
	}
		
	oldOpennedLimitSwitchValue = opennedLimitSwitchValue;
	oldClosedLimitSwitchValue = closedLimitSwitchValue;
	//end log

	if (!closedLimitSwitchValue) //limit has been reached
		return GateState::CLOSED;
	//else
	if (!opennedLimitSwitchValue) //limit has been reached
		return GateState::OPENED;
	//else
	return GateState::UNKNOWN;
}


void GateManager::GateMovementState::ChangeState(GateState newState) const
{
	_pGateManager->ChangeState(_pGateManager->SelectState(newState));
}

void GateManager::GateMovementStopping::Initialize()
{
	digitalWrite(flashingLED, LOW); //stop flashing LED
	ChangeState(GateState::STOPPED);
}

void GateManager::OnCommand(const String& commandName)
{
	GateMovementStateP_t newState = nullptr;
	if (commandName.equalsIgnoreCase("open"))
		newState = SelectState(GateState::OPENNING);
	else if (commandName.equalsIgnoreCase("close"))
		newState = SelectState(GateState::CLOSING);
	else if (commandName.equalsIgnoreCase("stop"))
		newState = SelectState(GateState::STOPPING);
	else if (commandName.equalsIgnoreCase("ButtonPressed"))
	{
		Serial.println("ButtonPressed command");
		OnButtonPressed();
		return;
	}
	else
		Serial.println("Unknown command");

	if (newState)
	{
		ChangeState(newState);
	}
}

void GateManager::OnButtonPressed()
{
	auto newState = _state->OnButtonPressed(_previousState);
	if (_state->State() == GateState::OPENNING || _state->State() == GateState::CLOSING)
		_previousState = _state->State(); //save previous movement state in case the gate stopes in the middle and we need to change direction
	ChangeState(SelectState(newState));
}

void GateManager::Loop()
{
	if (_state == nullptr)
	{
		Initialize();
	}

	_state->Loop();
}


