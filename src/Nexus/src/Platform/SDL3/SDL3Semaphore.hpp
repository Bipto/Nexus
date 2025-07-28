#pragma once

#include "Nexus-Core/Threading/Semaphore.hpp"
#include "SDL3Include.hpp"

namespace Nexus::Threading
{
	class SDL3Semaphore final : public SemaphoreBase
	{
	  public:
		SDL3Semaphore(uint32_t startingValue);
		virtual ~SDL3Semaphore();
		void	 Signal() final;
		void	 Wait() final;
		bool	 Wait(TimeSpan timeout) final;
		uint32_t GetValue() const final;

	  private:
		SDL_Semaphore *m_Semaphore = nullptr;
	};
}	 // namespace Nexus::Threading