#include "GateManager.h"


using namespace std;

GateManager::GateManager(function<void(const String &)> gateStatusCallback) : _gateStatusCallback(gateStatusCallback)
{
	Serial.println("Gate Manager has started");
}

void GateManager::Intialize(shared_ptr<GateManager> This) //called by the singleton create, after ctor
{
	_states = decltype(_states)
	{
		make_shared<GateMovementStandstillInTheMiddleState<GateState::UNKNOWN>>(This),
			make_shared<GateMovementOpenedClosedState<GateState::OPENED>>(This),
			make_shared<GateMovementOpennignOrClosing<OpeningDirection, GateState::OPENNING, GateState::OPENED>>(This),
			make_shared<GateMovementStopping>(This),
			make_shared<GateMovementStandstillInTheMiddleState<GateState::STOPPED>>(This),
			make_shared<GateMovementOpennignOrClosing<-OpeningDirection, GateState::CLOSING, GateState::CLOSED>>(This),
			make_shared<GateMovementOpenedClosedState<GateState::CLOSED>>(This)
	};

	_statuses = decltype(_statuses)
	{
		"Unknown",
		"Opened",
		"Opening",
		"Stopping",
		"Stopped",
		"Closing",
		"Closed"
	};
	pinMode(flashingLED, OUTPUT);
	ChangeState(SelectState(ReadState()));
}



void GateManager::ChangeState(shared_ptr<GateMovementState> state)
{
	
	Serial.printf("Gate change state. Old state: %s\t", _state ? _statuses[static_cast<unsigned char>(_state->State())].c_str() : "Initizlizing");
	_state = state;	auto newStatus = _statuses[static_cast<unsigned char>(_state->State())].c_str();
	Serial.printf("New state: %s\n", newStatus);
	
	if (_gateStatusCallback)
		_gateStatusCallback(newStatus);
	
	_state->Initialize();
}

GateState GateManager::ReadState()
{
	
	int limitSwitchesValue = analogRead(limitSwitches);
	Serial.println(limitSwitchesValue);
	//Use the A0 analog input to read resistor values. the closed limit switch has a 1/2 resistor ratio, the open limit switch as 2/3 ratio. Together they have 3/4
	bool limitSwitchGateClosedValue = (450 < limitSwitchesValue && limitSwitchesValue < 520) || limitSwitchesValue > 570; 
	bool limitSwitchGateOpenedValue = (300 < limitSwitchesValue && limitSwitchesValue < 370) || limitSwitchesValue > 570;

	if (limitSwitchGateClosedValue) //limit has been reached
		return GateState::CLOSED;
	//else
	if (limitSwitchGateOpenedValue) //limit has been reached
		return GateState::OPENED;
	//else
	return GateState::UNKNOWN;
}


void GateManager::GateMovementState::ChangeState(GateState newState) const
{
	auto gateManager = _gateManager.lock();
	gateManager->ChangeState(gateManager->SelectState(newState));
}

void GateManager::GateMovementStopping::Initialize()
{
	digitalWrite(flashingLED, LOW); //stop flashing LED
	ChangeState(GateState::STOPPED);
}

void GateManager::OnCommand(const String& commandName, int commandId)
{
	GateMovementStatePtr_t newState;
	if (commandName.equalsIgnoreCase("Open"))
		newState = SelectState(GateState::OPENNING);
	else if (commandName.equalsIgnoreCase("Close"))
		newState = SelectState(GateState::CLOSING);
	else if (commandName.equalsIgnoreCase("Stop"))
		newState = SelectState(GateState::STOPPING);
	else
		_gateStatusCallback("Unknown command");

	if (newState)
	{
		ChangeState(newState);
	}
}

void GateManager::OnButtonPressed()
{
	auto state = ReadState();
	
	auto newState = _state->OnButtonPressed(_previousState);
	if (_state->State() == GateState::OPENNING || _state->State() == GateState::CLOSING)
		_previousState = _state->State(); //save previous movement state in case the gate stopes in the middle and we need to change direction
	ChangeState(SelectState(newState));
}


