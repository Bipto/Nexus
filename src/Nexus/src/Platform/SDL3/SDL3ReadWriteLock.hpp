#pragma once

#include "Nexus-Core/Threading/ReadWriteLock.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3ReadWriteLock final : public ReadWriteLockBase
	{
	  public:
		SDL3ReadWriteLock();
		virtual ~SDL3ReadWriteLock() = default;
		void Lock(LockMode mode) final;
		bool TryLock(LockMode mode) final;
		void Unlock() final;

	  private:
		std::unique_ptr<SDL_RWLock, std::function<void(SDL_RWLock *)>> m_Lock = {};
	};
}	 // namespace Nexus::Threading