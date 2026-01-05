#pragma once

#include <memory>

#include "Platform/Timings/Timespan.hpp"

namespace Nexus::Threading
{
	class SemaphoreBase
	{
	  public:
		virtual void	 Signal()				  = 0;
		virtual void	 Wait()					  = 0;
		virtual bool	 Wait(uint32_t timeoutMS) = 0;
		virtual uint32_t GetValue() const		  = 0;
	};

	class Semaphore final
	{
	  public:
		Semaphore(uint32_t startingValue);
		virtual ~Semaphore() = default;
		void	 Signal();
		void	 Wait();
		bool	 Wait(uint32_t timeoutMS);
		uint32_t GetValue() const;

	  private:
		std::unique_ptr<SemaphoreBase> m_BaseSemaphore = nullptr;
	};

}	 // namespace Nexus::Threading