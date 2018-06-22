// ArduinoLoopManager.h

#ifndef _ARDUINOLOOPMANAGER_h
#define _ARDUINOLOOPMANAGER_h
//#include <Arduino.h>

#include "Singleton.h"
#include <memory>
#include <initializer_list>
#include <Arduino.h>

class IProcessor
{
public:
	virtual  void Loop() = 0;
	virtual const char *GetName() const = 0;
    IProcessor() = default;
    IProcessor(const IProcessor&) = delete;
    IProcessor& operator=(const IProcessor&) = delete;
    IProcessor(const IProcessor&&) = delete;
    IProcessor& operator=(const IProcessor&&) = delete;
    virtual ~IProcessor() = default;
};

typedef std::shared_ptr<IProcessor>processor_t;

class ArduinoLoopManager : public Singleton<ArduinoLoopManager>
{
	friend class Singleton<ArduinoLoopManager>;
private:
	std::vector<processor_t> _processors;
	ArduinoLoopManager(std::initializer_list<processor_t> processors) : _processors(processors) {}
public:
	void Loop() const
	{
		for (auto &&processor : _processors)
		{
			//Serial.print("Executing the loop of:");
			//Serial.println(processor->GetName());
			processor->Loop();
		}
	}
};

typedef std::shared_ptr<ArduinoLoopManager> ArduinoLoopManager_t;
#endif

