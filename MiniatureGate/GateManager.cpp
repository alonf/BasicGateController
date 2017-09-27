#include "GateManager.h"
#include <Wire.h>

using namespace std;

GateManager::GateManager(function<void(const String &)> gateStatusCallback) : _gateStatusCallback(gateStatusCallback)
{
	Serial.println("Gate Manager has started");
}

void GateManager::SetCommunicationDirection(CommunicationDirection direction)
{
	
	if (direction == CommunicationDirection::MasterToSlave)
	{
		pinMode(CommunicationLine0, OUTPUT);
		digitalWrite(CommunicationLine0, HIGH);

		pinMode(CommunicationLine1, OUTPUT);
		digitalWrite(CommunicationLine1, HIGH);

		pinMode(CommunicationLine2, OUTPUT);
		digitalWrite(CommunicationLine2, HIGH);

		pinMode(CommunicationLineDirection, OUTPUT);
		digitalWrite(CommunicationLineDirection, LOW);
	}
	else
	{
		pinMode(CommunicationLine0, INPUT_PULLUP);

		pinMode(CommunicationLine1, INPUT_PULLUP);

		pinMode(CommunicationLine2, INPUT_PULLUP);

		pinMode(CommunicationLineDirection, OUTPUT);
		digitalWrite(CommunicationLineDirection, HIGH);
	}

}

void GateManager::Intialize(shared_ptr<GateManager> This) //called by the singleton create, after ctor
{
	SetCommunicationDirection(CommunicationDirection::MasterToSlave);
}

void GateManager::OnCommand(const String& commandName)
{
	if (commandName.equalsIgnoreCase("Open"))
		Send(Command::Open);
	else if (commandName.equalsIgnoreCase("Close"))
		Send(Command::Close);
	else if (commandName.equalsIgnoreCase("Stop"))
		Send(Command::Stop);
}

void GateManager::OnButtonPressed()
{
	Send(Command::Button);
}

void GateManager::RecieveStatus()
{
	SetCommunicationDirection(CommunicationDirection::SlaveToMaster);
	delay(250); //status has to be ready in 0.25 seconds
	unsigned char statusBuilder = 0;
	statusBuilder |= digitalRead(CommunicationLine0);
	statusBuilder |= digitalRead(CommunicationLine1) << 1;
	statusBuilder |= digitalRead(CommunicationLine2) << 2;

	GateStatus status = static_cast<GateStatus>(statusBuilder);
	String statusMsg;
	switch (status)
	{
	case GateStatus::OpenedMessage:
		statusMsg = "Gate Open";
		break;
	case GateStatus::OpenningMessage:
		statusMsg = "Gate Openning";
		break;
	case GateStatus::StopedMessage:
		statusMsg = "Gate Movement Stopped";
		break;


	case GateStatus::ClosedMessage:
		statusMsg = "Gate Close";
		break;

	case GateStatus::ClosingMessage:
		statusMsg = "Gate Closing";
		break;

	case GateStatus::UnknownMessage:
		statusMsg = "Gate Status Unknown";
		break;

	case GateStatus::NoStatus:
	case GateStatus::NoStatus1:
	default:
		statusMsg = "Error getting gate status";
		status = GateStatus::NoStatus;
	}

	if (_lastKnownStatus != status)
	{
		Serial.printf("The gate status is: (%d) %s\n", statusBuilder, statusMsg.c_str());
		_lastKnownStatus = status;
	}
}

void GateManager::Loop()
{
	auto now = millis();
	if (now - _lastStatusCheck > StatusPollingInterval)
	{
		RecieveStatus();
		_lastStatusCheck = now;
	}
}

void GateManager::Send(Command command)
{
	Serial.printf("Sending command code %d\n", command);
	auto commandCode = static_cast<unsigned char>(command);
	SetCommunicationDirection(CommunicationDirection::MasterToSlave);
	digitalWrite(CommunicationLine0, commandCode & 0b001 ? HIGH : LOW);
	digitalWrite(CommunicationLine1, commandCode & 0b010 ? HIGH : LOW);
	digitalWrite(CommunicationLine2, commandCode & 0b100 ? HIGH : LOW);
	delay(100); 
}
