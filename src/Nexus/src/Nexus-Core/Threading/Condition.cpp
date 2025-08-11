#include "Nexus-Core/Threading/Condition.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus::Threading
{
	Condition::Condition()
	{
		m_ConditionBase = std::unique_ptr<ConditionBase>(Platform::CreateConditionBase());
	}

	void Condition::Signal()
	{
		m_ConditionBase->Signal();
	}

	void Condition::BroadCast()
	{
		m_ConditionBase->BroadCast();
	}

	void Condition::Wait(Mutex &mutex)
	{
		m_ConditionBase->Wait(mutex);
	}

	bool Condition::Wait(Mutex &mutex, TimeSpan timeout)
	{
		return m_ConditionBase->Wait(mutex, timeout);
	}
}	 // namespace Nexus::Threading