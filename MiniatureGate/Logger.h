// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#include "arduino.h"
#include "LedsLogger.h"
#include "WebServer.h"
#include <memory>
#include "Singleton.h"
#include "ArduinoLoopManager.h"

class Logger : public Singleton<Logger>, public IProcessor
{
	friend class Singleton<Logger>;
private:
	LedsLoggerPtr_t _ledsLogger;
	Logger(int redLedPin, int greenLedPin, int baudRate);
public:
	void Loop() override { _ledsLogger->Loop(); }
	void OnCommand(const String &commandName, int commandId) const;
	void WriteErrorMessage(const String &message, int blinks) const;
	void OnWiFiStatusChanged(const ConnectionStatus& status) const;
	void OnLongButtonPressDetection() const;
	void OnVeryLongButtonPressDetection() const;
	static void WriteMessage(const String& message);
	template<typename T>
	static void WriteMessage(const T& message) { Serial.println(message); }
	void TestLeds() const;
};

typedef std::shared_ptr<Logger> LoggerPtr_t;


#endif

