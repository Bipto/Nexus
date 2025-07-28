#include "Nexus-Core/Threading/Thread.hpp"

#include "Nexus-Core/Platform.hpp"

namespace Nexus
{
	void Thread::CreateBase(const ThreadDescription &description)
	{
		m_ThreadBase = std::unique_ptr<ThreadBase>(Platform::CreateThreadBase(description, m_Function));
	}

	const ThreadDescription &Thread::GetDescription() const
	{
		return m_ThreadBase->GetDescription();
	}

	ThreadState Thread::GetThreadState() const
	{
		return m_ThreadBase->GetThreadState();
	}

	void Thread::Wait() const
	{
		return m_ThreadBase->Wait();
	}

	void Thread::Detach() const
	{
		return m_ThreadBase->Detach();
	}
}	 // namespace Nexus
