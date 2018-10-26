// PubSub.h

#ifndef _PUBSUB_h
#define _PUBSUB_h
#include <functional>
#include <vector>

template<typename TOwner, typename ...Args>
class PubSub
{
	friend TOwner;
	using CommandNotificationPtr_t = std::function<void(Args...)>;
private:
	std::vector<CommandNotificationPtr_t> _subscribers;

	void NotifyAll(Args... args) const;


 public:
	 void Register(CommandNotificationPtr_t subscriber);
};


template <typename TOwner, typename ... Args>
void PubSub<TOwner, Args...>::Register(CommandNotificationPtr_t subscriber)
{
	_subscribers.push_back(subscriber);
}

template <typename TOwner, typename ... Args>
void PubSub<TOwner, Args...>::NotifyAll(Args... args) const
{
	for (auto subscriber : _subscribers)
	{
		subscriber(std::forward<Args>(args)...);
	}
}

#endif

