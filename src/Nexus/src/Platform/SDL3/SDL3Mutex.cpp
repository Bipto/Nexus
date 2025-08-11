#include "SDL3Mutex.hpp"

namespace Nexus::Threading
{
	SDL3Mutex::SDL3Mutex()
	{
		m_Mutex = SDL_CreateMutex();
	}

	SDL3Mutex::~SDL3Mutex()
	{
		SDL_DestroyMutex(m_Mutex);
	}

	void SDL3Mutex::Lock()
	{
		SDL_LockMutex(m_Mutex);
		m_LockCount++;
	}

	bool SDL3Mutex::TryLock()
	{
		if (SDL_TryLockMutex(m_Mutex))
		{
			m_LockCount++;
			return true;
		}

		return false;
	}

	void SDL3Mutex::Unlock()
	{
		SDL_UnlockMutex(m_Mutex);
		if (m_LockCount > 0)
		{
			m_LockCount--;
		}
	}

	uint32_t SDL3Mutex::GetLockCount() const
	{
		return m_LockCount;
	}

	SDL_Mutex *SDL3Mutex::GetHandle()
	{
		return m_Mutex;
	}
}	 // namespace Nexus::Threading