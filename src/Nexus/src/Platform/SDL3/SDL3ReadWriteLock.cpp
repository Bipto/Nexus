#include "SDL3ReadWriteLock.hpp"

namespace Nexus::Threading
{
	SDL3ReadWriteLock::SDL3ReadWriteLock()
	{
		m_Lock = {SDL_CreateRWLock(), SDL_DestroyRWLock};
	}

	void SDL3ReadWriteLock::Lock(LockMode mode)
	{
		switch (mode)
		{
			case LockMode::Read:
			{
				SDL_LockRWLockForReading(m_Lock.get());
				break;
			}
			case LockMode::Write:
			{
				SDL_LockRWLockForReading(m_Lock.get());
				break;
			}
			default: throw std::runtime_error("Failed to find a valid lock mode");
		}
	}

	bool SDL3ReadWriteLock::TryLock(LockMode mode)
	{
		switch (mode)
		{
			case LockMode::Read:
			{
				return SDL_TryLockRWLockForReading(m_Lock.get());
			}
			case LockMode::Write:
			{
				return SDL_TryLockRWLockForWriting(m_Lock.get());
			}
			default: throw std::runtime_error("Failed to find a valid lock mode");
		}
	}

	void SDL3ReadWriteLock::Unlock()
	{
		SDL_UnlockRWLock(m_Lock.get());
	}
}	 // namespace Nexus::Threading