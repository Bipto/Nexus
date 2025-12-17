#include "SDL3Mutex.hpp"

namespace Nexus::Threading
{
	SDL3Mutex::SDL3Mutex()
	{
		m_Mutex = {SDL_CreateMutex(), SDL_DestroyMutex};
		SDL_SetAtomicInt(&m_LockCount, 0);
	}

	void SDL3Mutex::Lock()
	{
		SDL_LockMutex(m_Mutex.get());
		SDL_AtomicIncRef(&m_LockCount);
	}

	bool SDL3Mutex::TryLock()
	{
		if (SDL_TryLockMutex(m_Mutex.get()))
		{
			SDL_AtomicIncRef(&m_LockCount);
			return true;
		}

		return false;
	}

	void SDL3Mutex::Unlock()
	{
		SDL_UnlockMutex(m_Mutex.get());
		int lockValue = SDL_GetAtomicInt(&m_LockCount);
		if (lockValue > 0)
		{
			SDL_AtomicDecRef(&m_LockCount);
		}
	}

	uint32_t SDL3Mutex::GetLockCount() const
	{
		int lockValue = SDL_GetAtomicInt(&m_LockCount);
		return static_cast<uint32_t>(lockValue);
	}

	SDL_Mutex *SDL3Mutex::GetHandle()
	{
		return m_Mutex.get();
	}
}	 // namespace Nexus::Threading