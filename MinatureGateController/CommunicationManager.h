// CommunicationManager.h

#ifndef _COMMUNICATIONMANAGER_h
#define _COMMUNICATIONMANAGER_h

enum class CommunicationDirection : unsigned char
{
	MasterToSlave = LOW,
	SlaveToMaster = HIGH
};


class ICommand 
{
public:
	virtual void OnCommand(const String &command) = 0;
	virtual ~ICommand() = default;
};
typedef ICommand* ICommandP_t;


class CommunicationManager 
{
private:
	ICommandP_t _pCommandSubscriber;
	Command _lastCommand = Command::None;
	GateStatus _gateStatus = GateStatus::NoStatus;
	mutable unsigned char _previousSentStatusCode = 0;
	void OnCommand(const String& command) const;
	
	void ExecuteCommand();
	void SendStatus() const;
	void HandleCommunication();

public:
	void Loop();
	CommunicationManager(ICommandP_t commandSubscriber);
	void SetGateStatus(GateStatus status) { _gateStatus = status; }
};

typedef CommunicationManager * CommunicationManagerP_t;

#endif

