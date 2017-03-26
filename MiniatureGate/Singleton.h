// Singleton.h

#ifndef _SINGLETON_h
#define _SINGLETON_h
#include "arduino.h"
#include <memory>

template<typename T>
class Singleton 
{
private:
	static std::weak_ptr<T> _instance;

public:
	virtual ~Singleton() = default;

	template<typename... Args>
	static std::shared_ptr<T> Create(Args&&... args)
	{
		auto instance = _instance.lock();
		if (!instance)
		{
			// ReSharper disable CppSmartPointerVsMakeFunction
			instance = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
			// ReSharper restore CppSmartPointerVsMakeFunction
			_instance = instance;

			//support after ctor initialization
			instance->Intialize(instance);
		}
		return instance;
	}

	static std::shared_ptr<T> Instance()
	{
		return _instance.lock();
	}

	virtual void Intialize(std::shared_ptr<T> instance) {};

	Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};

template<typename T>
std::weak_ptr<T> Singleton<T>::_instance;

#endif