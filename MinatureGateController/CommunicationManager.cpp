#include <Arduino.h>
#include "Configuration.h"
#include "CommunicationManager.h"

CommunicationManager::CommunicationManager(ICommandP_t commandSubscriber) : _pCommandSubscriber(commandSubscriber)
{
	pinMode(CommunicationLineDirection, INPUT_PULLUP);
}


void CommunicationManager::OnCommand(const String& command) const
{
	Serial.write("Command: ");
	Serial.println(command);

	if (_pCommandSubscriber)
		_pCommandSubscriber->OnCommand(command);
}

void CommunicationManager::HandleCommunication()
{
	CommunicationDirection communicationDirection = digitalRead(CommunicationLineDirection) ? 
		CommunicationDirection::SlaveToMaster : CommunicationDirection::MasterToSlave;

	if (communicationDirection == CommunicationDirection::MasterToSlave)
	{
		pinMode(CommunicationLine0, INPUT_PULLUP);
		pinMode(CommunicationLine1, INPUT_PULLUP);
		pinMode(CommunicationLine2, INPUT_PULLUP);
		delay(50); //wait for the lines to become stable
		ExecuteCommand();
	}
	else
	{
		pinMode(CommunicationLine0, OUTPUT);
		pinMode(CommunicationLine1, OUTPUT);
		pinMode(CommunicationLine2, OUTPUT);
		SendStatus();
	}
}

void CommunicationManager::ExecuteCommand()
{
	unsigned char commandBuilder = 0;
	commandBuilder |= digitalRead(CommunicationLine0);
	commandBuilder |= digitalRead(CommunicationLine1) << 1;
	commandBuilder |= digitalRead(CommunicationLine2) << 2;

	Command command = static_cast<Command>(commandBuilder);
	if (command == _lastCommand)
		return; //noting to do

	_lastCommand = command;
	Serial.print("Command code: ");
	Serial.println(commandBuilder);

	switch (command)
	{
	case Command::Open:
		OnCommand("open");
		break;
	case Command::Close:
		OnCommand("close");
		break;
	case Command::Stop:
		OnCommand("stop");
		break;
	case Command::Button:
		OnCommand("buttonPressed");
		break;
	default:
		Serial.println("command cleared");
	}
}


void CommunicationManager::SendStatus() const
{
	auto statusCode = static_cast<unsigned char>(_gateStatus);
	if (statusCode != _previousSentStatusCode)
	{
		Serial.print("Sending status code:");
		Serial.println(statusCode);
		_previousSentStatusCode = statusCode;
	}

	digitalWrite(CommunicationLine0, statusCode & 0b001 ? HIGH : LOW);
	digitalWrite(CommunicationLine1, statusCode & 0b010 ? HIGH : LOW);
	digitalWrite(CommunicationLine2, statusCode & 0b100 ? HIGH : LOW);
}

int x;

void CommunicationManager::Loop()
{
	if (++x % 100000 == 0)
		Serial.write(".");

	HandleCommunication();
}

