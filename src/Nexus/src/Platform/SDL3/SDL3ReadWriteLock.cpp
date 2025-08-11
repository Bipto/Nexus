#include "SDL3ReadWriteLock.hpp"

namespace Nexus::Threading
{
	SDL3ReadWriteLock::SDL3ReadWriteLock()
	{
		m_Lock = SDL_CreateRWLock();
	}

	SDL3ReadWriteLock::~SDL3ReadWriteLock()
	{
		SDL_DestroyRWLock(m_Lock);
	}

	void SDL3ReadWriteLock::Lock(LockMode mode)
	{
		switch (mode)
		{
			case LockMode::Read:
			{
				SDL_LockRWLockForReading(m_Lock);
				break;
			}
			case LockMode::Write:
			{
				SDL_LockRWLockForReading(m_Lock);
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
				return SDL_TryLockRWLockForReading(m_Lock);
			}
			case LockMode::Write:
			{
				return SDL_TryLockRWLockForWriting(m_Lock);
			}
			default: throw std::runtime_error("Failed to find a valid lock mode");
		}
	}

	void SDL3ReadWriteLock::Unlock()
	{
		SDL_UnlockRWLock(m_Lock);
	}
}	 // namespace Nexus::Threading