#pragma once

#include "Nexus-Core/Threading/ReadWriteLock.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3ReadWriteLock final : public ReadWriteLockBase
	{
	  public:
		SDL3ReadWriteLock();
		virtual ~SDL3ReadWriteLock();
		void Lock(LockMode mode) final;
		bool TryLock(LockMode mode) final;
		void Unlock() final;

	  private:
		SDL_RWLock *m_Lock = nullptr;
	};
}	 // namespace Nexus::Threading