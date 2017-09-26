#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include "Configuration.h"
#include "GateManager.h"
#include "CommunicationManager.h"

using namespace std;



GateManagerP_t pGateManager;
CommunicationManagerP_t pCommunicationManager;

class CommandHandler : public ICommand
{
	virtual void OnCommand(const String &command) override
	{
		pGateManager->OnCommand(command);
	}
};



void setup() 
{
	Serial.begin(115000);
	Serial.println("Started.");
	static GateManager gateManager;
	pGateManager = &gateManager;

	static CommandHandler commandHandler;
	static CommunicationManager communicationManager(&commandHandler);
	pCommunicationManager = &communicationManager;
	
}


void loop() 
{
	pGateManager->Loop();
	pCommunicationManager->Loop();

	auto statusReportCode = pGateManager->StatusReportCode();

	pCommunicationManager->SetGateStatus(statusReportCode);
}
