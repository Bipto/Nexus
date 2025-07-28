#pragma once

#include "Nexus-Core/Threading/Mutex.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3Mutex : public MutexBase
	{
	  public:
		SDL3Mutex();
		virtual ~SDL3Mutex();
		void	   Lock() final;
		bool	   TryLock() final;
		void	   Unlock() final;
		uint32_t   GetLockCount() const final;
		SDL_Mutex *GetHandle();

	  private:
		SDL_Mutex			 *m_Mutex	  = nullptr;
		std::atomic<uint32_t> m_LockCount = 0;
	};
}	 // namespace Nexus::Threading