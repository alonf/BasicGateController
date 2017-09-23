#include <Arduino.h>
#include "Configuration.h"
#include "CommunicationManager.h"

void CommunicationManager::OnCommand(const String& command) const
{
	Serial.write("Command: ");
	Serial.println(command);

	if (_pCommandSubscriber)
		_pCommandSubscriber->OnCommand(command);
}

int x;

void CommunicationManager::Loop()
{
	if (++x % 100000 == 0)
		Serial.write(".");

	if (_open.IsTriggered())
		OnCommand(_open.Command());

	if (_close.IsTriggered())
		OnCommand(_close.Command());

	if (_stop.IsTriggered())
		OnCommand(_stop.Command());

	if (_buttonPressed.IsTriggered())
		OnCommand(_buttonPressed.Command());
}
