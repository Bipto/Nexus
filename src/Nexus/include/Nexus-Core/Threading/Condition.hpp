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

		virtual void Signal()							  = 0;
		virtual void BroadCast()						  = 0;
		virtual void Wait(Mutex &mutex)					  = 0;
		virtual bool Wait(Mutex &mutex, TimeSpan timeout) = 0;
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