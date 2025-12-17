#pragma once

#include "Nexus-Core/Threading/Mutex.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Threading
{
	class NX_API ConditionBase
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

	class NX_API Condition final
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