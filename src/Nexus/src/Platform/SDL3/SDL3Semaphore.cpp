#include "SDL3Semaphore.hpp"

namespace Nexus::Threading
{
	SDL3Semaphore::SDL3Semaphore(uint32_t startingValue)
	{
		m_Semaphore = SDL_CreateSemaphore(startingValue);
	}

	SDL3Semaphore::~SDL3Semaphore()
	{
		SDL_DestroySemaphore(m_Semaphore);
	}

	void SDL3Semaphore::Signal()
	{
		SDL_SignalSemaphore(m_Semaphore);
	}

	void SDL3Semaphore::Wait()
	{
		SDL_WaitSemaphore(m_Semaphore);
	}

	bool SDL3Semaphore::Wait(TimeSpan timeout)
	{
		return SDL_WaitSemaphoreTimeout(m_Semaphore, timeout.GetMilliseconds<int32_t>());
	}

	uint32_t SDL3Semaphore::GetValue() const
	{
		return SDL_GetSemaphoreValue(m_Semaphore);
	}
}	 // namespace Nexus::Threading