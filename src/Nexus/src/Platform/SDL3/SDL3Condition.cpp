#include "SDL3Condition.hpp"
#include "SDL3Mutex.hpp"

namespace Nexus::Threading
{
	Nexus::Threading::SDL3Condition::SDL3Condition()
	{
		m_Condition = {SDL_CreateCondition(), SDL_DestroyCondition};
	}

	void SDL3Condition::Signal() const
	{
		SDL_SignalCondition(m_Condition.get());
	}

	void SDL3Condition::BroadCast() const
	{
		SDL_BroadcastCondition(m_Condition.get());
	}

	void SDL3Condition::Wait(const Mutex &mutex) const
	{
		MutexBase *base		= mutex.GetBase();
		SDL3Mutex *sdlMutex = (SDL3Mutex *)base;
		SDL_WaitCondition(m_Condition.get(), sdlMutex->GetHandle());
	}

	bool SDL3Condition::Wait(const Mutex &mutex, TimeSpan timeout) const
	{
		MutexBase *base		= mutex.GetBase();
		SDL3Mutex *sdlMutex = (SDL3Mutex *)base;
		return SDL_WaitConditionTimeout(m_Condition.get(), sdlMutex->GetHandle(), timeout.GetMilliseconds<int32_t>());
	}
}	 // namespace Nexus::Threading
