#pragma once

#include <cstdint>
#include <memory>

#include "Platform/Platform-Core.hpp"

namespace Nexus::Threading
{
	class NX_PLATFORM_API MutexBase
	{
	  public:
		virtual ~MutexBase()				  = default;
		virtual void	 Lock()				  = 0;
		virtual bool	 TryLock()			  = 0;
		virtual void	 Unlock()			  = 0;
		virtual uint32_t GetLockCount() const = 0;
	};

	class NX_PLATFORM_API Mutex final
	{
	  public:
		Mutex();
		~Mutex() = default;
		void	   Lock();
		bool	   TryLock();
		void	   Unlock();
		uint32_t   GetLockCount() const;
		MutexBase *GetBase() const;

	  private:
		std::unique_ptr<MutexBase> m_BaseMutex = nullptr;
	};

	class NX_PLATFORM_API LockGuard final
	{
	  public:
		explicit LockGuard(Mutex &mutex);
		virtual ~LockGuard();
		LockGuard(const LockGuard &)			= delete;
		LockGuard &operator=(const LockGuard &) = delete;

	  private:
		Mutex &m_Mutex;
	};
}	 // namespace Nexus::Threading