#include "Nexus-Core/Threading/Semaphore.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus::Threading
{
	Semaphore::Semaphore(uint32_t startingValue)
	{
		m_BaseSemaphore = std::unique_ptr<SemaphoreBase>(Platform::CreateSemaphoreBase(startingValue));
	}

	void Semaphore::Signal()
	{
		m_BaseSemaphore->Signal();
	}

	void Semaphore::Wait()
	{
		m_BaseSemaphore->Wait();
	}

	bool Semaphore::Wait(TimeSpan timeout)
	{
		return m_BaseSemaphore->Wait(timeout);
	}

	uint32_t Semaphore::GetValue() const
	{
		return m_BaseSemaphore->GetValue();
	}
}	 // namespace Nexus::Threading