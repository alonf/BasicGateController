// GateManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include "Configuration.h"
#include <Stepper.h>

enum class GateState : unsigned char 
{ 
	UNKNOWN, OPENED, OPENNING, STOPPING, STOPPED, CLOSING, CLOSED, MAX_VALUE = CLOSED
};

class GateManager final : public Singleton<GateManager>, public IProcessor
{
	friend class Singleton<GateManager>;
private:
	class GateMovementState;
	void ChangeState(std::shared_ptr<GateMovementState> state);
	static GateState ReadState();
	virtual void Intialize(std::shared_ptr<GateManager> This) override;

	class GateMovementState
	{
	protected:
		void ChangeState(GateState newState) const;
	public:
		explicit GateMovementState(std::shared_ptr<GateManager> gateManager) : _gateManager(gateManager) {}
		virtual void Loop() {}
		virtual ~GateMovementState() = default;
		virtual GateState State() const = 0;
		virtual void Initialize() {}
		virtual GateState OnButtonPressed(GateState previousMovement) = 0;
		std::weak_ptr<GateManager> _gateManager;
	};

	template<GateState currentState>
	class GateMovementStandstillInTheMiddleState final : public GateMovementState
	{
	public:
		explicit GateMovementStandstillInTheMiddleState(std::shared_ptr<GateManager> gateManager) : GateMovementState(gateManager) {}
	private:
		GateState State() const override { return currentState; }
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return previousMovement == GateState::OPENNING ? GateState::CLOSING : GateState::OPENNING;
		}
	};

	template<GateState currentState>
	class GateMovementOpenedClosedState final : public GateMovementState
	{
	public:
		explicit GateMovementOpenedClosedState(std::shared_ptr<GateManager> gateManager) : GateMovementState(gateManager) {}
	private:
		GateState State() const override { return currentState; }
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return currentState == GateState::OPENED ? GateState::CLOSING : GateState::OPENNING;
		}
	};

	class GateMovementStopping final : public GateMovementState
	{
	public:
		explicit GateMovementStopping(std::shared_ptr<GateManager> gateManager) : GateMovementState(gateManager) {}
	private:
		GateState State() const override { return GateState::STOPPING; }
		virtual void Initialize() override;
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return previousMovement == GateState::OPENNING ? GateState::CLOSING : GateState::OPENNING;
		}
	};

	template<int Direction, GateState CurrentState, GateState EndState>
	class GateMovementOpennignOrClosing : public GateMovementState
	{
	public:
		explicit GateMovementOpennignOrClosing(std::shared_ptr<GateManager> gateManager) : 
			GateMovementState(gateManager), 
			_stepper(stepsPerRevolution, stepper1, stepper2, stepper3, stepper4)
		{
			_stepper.setSpeed(stepperSpeed);
		}

	private:
		Stepper _stepper;
		int _flashingLedTime = 0;
		int _flashingLedState = LOW;

		GateState State() const override { return CurrentState; }
		virtual void Initialize() override;
		void Loop() override;
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return GateState::STOPPING;
		}
	};

	typedef std::shared_ptr<GateMovementState> GateMovementStatePtr_t;
	GateMovementStatePtr_t _state;
	std::function<void(const String &)> _gateStatusCallback;
	GateState _previousState = GateState::UNKNOWN;
	explicit GateManager(std::function<void(const String &)> gateStatusCallback);
	
	std::array<GateMovementStatePtr_t, static_cast<size_t>(GateState::MAX_VALUE) + 1> _states;
	std::array<String, static_cast<size_t>(GateState::MAX_VALUE) + 1> _statuses;

	GateMovementStatePtr_t SelectState(GateState state) const
	{
		return _states[static_cast<unsigned char>(state)];
	}
	GateState State() const { return _state->State(); }

public:
	
	void OnCommand(const String & commandName, int commandId);
	const String &Status() const { return _statuses[static_cast<unsigned char>(_state->State())]; }
	void OnButtonPressed();
	void Loop() override { _state->Loop(); }
};



template <int Direction, GateState CurrentState, GateState EndState>
void GateManager::GateMovementOpennignOrClosing<Direction, CurrentState, EndState>::Initialize()
{
	_flashingLedState = HIGH;
	_flashingLedTime = millis();
}

template <int Direction, GateState CurrentState, GateState EndState>
void GateManager::GateMovementOpennignOrClosing<Direction, CurrentState, EndState>::Loop()
{
	//handle flashing led blinks
	if (millis() - _flashingLedTime > flashingLedPeriod)
	{
		_flashingLedState = _flashingLedState == HIGH ? LOW : HIGH; //toggle LED state
		digitalWrite(flashingLED, _flashingLedState);
		_flashingLedTime = millis();
	}
	auto gateManger = _gateManager.lock();
	GateState currentState = gateManger->ReadState();
	if (currentState == EndState) //opened or closed
	{
		ChangeState(currentState);
		digitalWrite(flashingLED, LOW);
		return;
	}
	_stepper.step(Direction * stepsPerRevolution / 100);
}

typedef std::shared_ptr<GateManager> GateManagerPtr_t;
#endif

