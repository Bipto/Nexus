#pragma once

#include "Nexus-Core/Threading/Mutex.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3Mutex final : public MutexBase
	{
	  public:
		SDL3Mutex();
		virtual ~SDL3Mutex() = default;
		void	   Lock() final;
		bool	   TryLock() final;
		void	   Unlock() final;
		uint32_t   GetLockCount() const final;
		SDL_Mutex *GetHandle();

	  private:
		std::unique_ptr<SDL_Mutex, std::function<void(SDL_Mutex *)>> m_Mutex = {};

		/// @brief An atomic integer, used to keep track of the lock count
		/// @note This is necessary because SDL_Mutex does not provide a way to a count and has to be mutable because a non-const pointer is needed to
		/// retrieve the value, despite not modifying it
		mutable SDL_AtomicInt m_LockCount = {};
	};
}	 // namespace Nexus::Threading