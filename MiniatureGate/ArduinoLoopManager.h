// ArduinoLoopManager.h

#ifndef _ARDUINOLOOPMANAGER_h
#define _ARDUINOLOOPMANAGER_h
#include <functional>
#include <initializer_list>
#include <memory>
#include "Singleton.h"
#include<vector>

class IProcessor
{
public:
	virtual  void Loop() = 0;
	virtual ~IProcessor() {}
};

typedef std::shared_ptr<IProcessor> processor_t;

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
			processor->Loop();
		}
	}
};

typedef std::shared_ptr<ArduinoLoopManager> ArduinoLoopManager_t;
#endif

