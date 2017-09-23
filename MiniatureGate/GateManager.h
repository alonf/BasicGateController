// GateManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include "Configuration.h"



class GateManager final : public Singleton<GateManager>, public IProcessor
{
	friend class Singleton<GateManager>;
private:
	class GateMovementState;
	
	virtual void Intialize(std::shared_ptr<GateManager> This) override;
	explicit GateManager(std::function<void(const String &)> gateStatusCallback);
	std::function<void(const String &)> _gateStatusCallback;
	

public:

	static void Send(int command);
	static void OnCommand(const String & commandName);
	static void OnButtonPressed();
	void Loop() override {  }
};


typedef std::shared_ptr<GateManager> GateManagerPtr_t;
#endif

