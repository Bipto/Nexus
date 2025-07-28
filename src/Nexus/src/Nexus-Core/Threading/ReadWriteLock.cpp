#include "Nexus-Core/Threading/ReadWriteLock.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus::Threading
{
	ReadWriteLock::ReadWriteLock()
	{
		m_BaseReadWriteLock = std::unique_ptr<ReadWriteLockBase>(Platform::CreateReadWriteLockBase());
	}

	void ReadWriteLock::Lock(LockMode mode)
	{
		m_BaseReadWriteLock->Lock(mode);
	}

	bool ReadWriteLock::TryLock(LockMode mode)
	{
		return m_BaseReadWriteLock->TryLock(mode);
	}

	void ReadWriteLock::Unlock()
	{
		m_BaseReadWriteLock->Unlock();
	}
}	 // namespace Nexus::Threading