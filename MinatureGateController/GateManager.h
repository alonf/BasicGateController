// GateManager.h

#ifndef _RELAYMANAGER_h
#define _RELAYMANAGER_h
#include "arduino.h"
#include "Configuration.h"
#include <Stepper.h>
#include "CommunicationManager.h"

enum class GateState : unsigned char 
{ 
	UNKNOWN, OPENED, OPENNING, STOPPING, STOPPED, CLOSING, CLOSED, MAX_VALUE = CLOSED
};

class GateManager final 
{
	private:
	class GateMovementState;
	void ChangeState(GateMovementState *pState);
	void Initialize();
	
	static GateState ReadState();
	
	class GateMovementState
	{
	protected:
		void ChangeState(GateState newState) const;
	public:
		explicit GateMovementState(GateManager *pGateManager) : _pGateManager(pGateManager) {}
		virtual void Loop() {}
		virtual ~GateMovementState() {}
		virtual GateState State() const = 0;
		virtual void Initialize() {}
		virtual GateState OnButtonPressed(GateState previousMovement) = 0;
		virtual GateStatus StatusReportCode() const = 0;
		GateManager *_pGateManager;
	};

	template<GateState currentState>
	class GateMovementStandstillInTheMiddleState final : public GateMovementState
	{
	public:
		using GateMovementState::GateMovementState;
	private:
		GateState State() const override { return currentState; }
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return previousMovement == GateState::OPENNING ? GateState::CLOSING : GateState::OPENNING;
		}

		GateStatus StatusReportCode() const override { return GateStatus::StopedMessage; }
	};

	template<GateState currentState>
	class GateMovementOpenedClosedState final : public GateMovementState
	{
	public:
		using GateMovementState::GateMovementState;
	private:
		GateState State() const override { return currentState; }
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return currentState == GateState::OPENED ? GateState::CLOSING : GateState::OPENNING;
		}
		GateStatus StatusReportCode() const override { return currentState == GateState::OPENED ? GateStatus::OpenedMessage : GateStatus::ClosedMessage; }
	};

	class GateMovementStopping final : public GateMovementState
	{
	public:
		using GateMovementState::GateMovementState;
	private:
		GateState State() const override { return GateState::STOPPING; }
		GateStatus StatusReportCode() const override { return GateStatus::StopedMessage; }
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
		explicit GateMovementOpennignOrClosing(GateManager *pGateManager) : 
			GateMovementState(pGateManager),
			_stepper(stepsPerRevolution, stepper1, stepper2, stepper3, stepper4)
		{
			_stepper.setSpeed(stepperSpeed);
		}
		GateStatus StatusReportCode() const override { return CurrentState == GateState::OPENNING ? GateStatus::OpenningMessage : GateStatus::ClosingMessage; }

	private:
		Stepper _stepper;
		unsigned long _flashingLedTime = 0;
		int _flashingLedState = LOW;

		GateState State() const override { return CurrentState; }
		virtual void Initialize() override;
		void Loop() override;
		GateState OnButtonPressed(GateState previousMovement) override
		{
			return GateState::STOPPING;
		}
	};

	typedef GateMovementState *GateMovementStateP_t;
	GateMovementStateP_t _state;
	GateState _previousState = GateState::UNKNOWN;

	
	GateMovementStateP_t _states[static_cast<size_t>(GateState::MAX_VALUE) + 1];
	String _statuses[static_cast<size_t>(GateState::MAX_VALUE) + 1];

	GateMovementStateP_t SelectState(GateState state) const
	{
		return _states[static_cast<unsigned char>(state)];
	}
	GateState State() const { return _state->State(); }

public:
	explicit GateManager();
	void OnCommand(const String & commandName);
	const String &Status() const { return _statuses[static_cast<unsigned char>(_state->State())]; }
	GateStatus StatusReportCode() const { return _state->StatusReportCode(); }
	void OnButtonPressed();
	void Loop();
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
	auto currentTick = millis();
	//handle flashing led blinks
	if (currentTick - _flashingLedTime > flashingLedPeriod)
	{
		_flashingLedState = _flashingLedState == HIGH ? LOW : HIGH; //toggle LED state
		digitalWrite(flashingLED, _flashingLedState);
		_flashingLedTime = currentTick;
	}
	
	GateState currentState = _pGateManager->ReadState();
	if (currentState == EndState) //opened or closed
	{
		ChangeState(currentState);
		digitalWrite(flashingLED, LOW);
		return;
	}
	_stepper.step(Direction * stepsPerRevolution / 100);
}

typedef GateManager *GateManagerP_t;
#endif

