#include "GateManager.h"
#include <Wire.h>

using namespace std;

GateManager::GateManager(function<void(const String &)> gateStatusCallback) : _gateStatusCallback(gateStatusCallback)
{
	Serial.println("Gate Manager has started");
}


void GateManager::Intialize(shared_ptr<GateManager> This) //called by the singleton create, after ctor
{
	pinMode(OpenCommand, OUTPUT);
	digitalWrite(OpenCommand, HIGH);

	pinMode(CloseCommand, OUTPUT);
	digitalWrite(CloseCommand, HIGH);

	pinMode(StopCommand, OUTPUT);
	digitalWrite(StopCommand, HIGH);

	pinMode(ButtonPressedCommand, OUTPUT);
	digitalWrite(ButtonPressedCommand, HIGH);
}

void GateManager::OnCommand(const String& commandName)
{
	if (commandName.equalsIgnoreCase("Open"))
		Send(OpenCommand);
	else if (commandName.equalsIgnoreCase("Close"))
		Send(CloseCommand);
	else if (commandName.equalsIgnoreCase("Stop"))
		Send(StopCommand);
}

void GateManager::OnButtonPressed()
{
	Send(ButtonPressedCommand);
}

void GateManager::Send(int command)
{
	Serial.printf("Sending command on pin %d\n", command);
	digitalWrite(command, LOW);
	delay(100);
	digitalWrite(command, HIGH);
}
