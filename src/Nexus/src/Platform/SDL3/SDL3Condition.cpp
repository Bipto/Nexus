#include "SDL3Condition.hpp"
#include "SDL3Mutex.hpp"

namespace Nexus::Threading
{
	Nexus::Threading::SDL3Condition::SDL3Condition()
	{
		m_Condition = SDL_CreateCondition();
	}

	SDL3Condition::~SDL3Condition()
	{
		SDL_DestroyCondition(m_Condition);
	}

	void SDL3Condition::Signal()
	{
		SDL_SignalCondition(m_Condition);
	}

	void SDL3Condition::BroadCast()
	{
		SDL_BroadcastCondition(m_Condition);
	}

	void SDL3Condition::Wait(Mutex &mutex)
	{
		MutexBase *base		= mutex.GetBase();
		SDL3Mutex *sdlMutex = (SDL3Mutex *)base;
		SDL_WaitCondition(m_Condition, sdlMutex->GetHandle());
	}

	bool SDL3Condition::Wait(Mutex &mutex, TimeSpan timeout)
	{
		MutexBase *base		= mutex.GetBase();
		SDL3Mutex *sdlMutex = (SDL3Mutex *)base;
		return SDL_WaitConditionTimeout(m_Condition, sdlMutex->GetHandle(), timeout.GetMilliseconds<int32_t>());
	}
}	 // namespace Nexus::Threading
