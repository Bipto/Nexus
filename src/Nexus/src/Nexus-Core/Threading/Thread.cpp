#include "Nexus-Core/Threading/Thread.hpp"

#include "Nexus-Core/Platform.hpp"

namespace Nexus
{
	void Thread::CreateBase(const ThreadDescription &description)
	{
		m_ThreadBase = std::unique_ptr<ThreadBase>(Platform::CreateThreadBase(description, m_Function));
	}
}	 // namespace Nexus
