#include "Nexus-Core/Threading/Mutex.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus::Threading
{
	Mutex::Mutex()
	{
		m_BaseMutex = std::unique_ptr<MutexBase>(Platform::CreateMutexBase());
	}

	void Mutex::Lock()
	{
		m_BaseMutex->Lock();
	}

	bool Mutex::TryLock()
	{
		return m_BaseMutex->TryLock();
	}

	void Mutex::Unlock()
	{
		return m_BaseMutex->Unlock();
	}

	uint32_t Mutex::GetLockCount() const
	{
		return m_BaseMutex->GetLockCount();
	}

	MutexBase *Mutex::GetBase() const
	{
		return m_BaseMutex.get();
	}

	LockGuard::LockGuard(Mutex &mutex) : m_Mutex(mutex)
	{
		m_Mutex.Lock();
	}

	LockGuard::~LockGuard()
	{
		m_Mutex.Unlock();
	}
}	 // namespace Nexus::Threading
