// GateManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h

#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include "Configuration.h"
#include <Arduino.h>

enum class CommunicationDirection : unsigned char
{
	MasterToSlave = LOW,
	SlaveToMaster = HIGH
};

class GateManager final : public Singleton<GateManager>, public IProcessor
{
	friend class Singleton<GateManager>;
private:
	class GateMovementState;
	
	virtual void Intialize(std::shared_ptr<GateManager> This) override;
	explicit GateManager(std::function<void(const String &)> gateStatusCallback);
	static void SetCommunicationDirection(CommunicationDirection direction);
	std::function<void(const String &)> _gateStatusCallback;
	unsigned long _lastStatusCheck = 0;
	GateStatus _lastKnownStatus = GateStatus::NoStatus;
	static void Send(Command command);
	void RecieveStatus();

public:
	static void OnButtonPressed();
	static void OnCommand(const String & commandName);
	void Loop() override;
};


typedef std::shared_ptr<GateManager> GateManagerPtr_t;
#endif

