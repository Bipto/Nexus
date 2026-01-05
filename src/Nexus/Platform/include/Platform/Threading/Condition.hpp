#pragma once

#include "Platform/Platform-Core.hpp"
#include "Platform/Threading/Mutex.hpp"
#include "Platform/Timings/Timespan.hpp"

namespace Nexus::Threading
{
	class NX_PLATFORM_API ConditionBase
	{
	  public:
		virtual ~ConditionBase()
		{
		}

		virtual void Signal() const									  = 0;
		virtual void BroadCast() const								  = 0;
		virtual void Wait(const Mutex &mutex) const					  = 0;
		virtual bool Wait(const Mutex &mutex, TimeSpan timeout) const = 0;
	};

	class NX_PLATFORM_API Condition final
	{
	  public:
		Condition();
		virtual ~Condition() = default;
		void Signal();
		void BroadCast();
		void Wait(Mutex &mutex);
		bool Wait(Mutex &mutex, TimeSpan timeout);

	  private:
		std::unique_ptr<ConditionBase> m_ConditionBase = nullptr;
	};
}	 // namespace Nexus::Threading