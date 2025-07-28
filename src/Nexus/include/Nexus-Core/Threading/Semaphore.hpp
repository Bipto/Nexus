#pragma once

#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Threading
{
	class SemaphoreBase
	{
	  public:
		virtual void	 Signal()				= 0;
		virtual void	 Wait()					= 0;
		virtual bool	 Wait(TimeSpan timeout) = 0;
		virtual uint32_t GetValue() const		= 0;
	};

	class Semaphore final
	{
	  public:
		Semaphore(uint32_t startingValue);
		virtual ~Semaphore() = default;
		void	 Signal();
		void	 Wait();
		bool	 Wait(TimeSpan timeout);
		uint32_t GetValue() const;

	  private:
		std::unique_ptr<SemaphoreBase> m_BaseSemaphore = nullptr;
	};

}	 // namespace Nexus::Threading