#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Threading
{
	enum class LockMode
	{
		Read,
		Write
	};

	class ReadWriteLockBase
	{
	  public:
		virtual ~ReadWriteLockBase()		= default;
		virtual void Lock(LockMode mode)	= 0;
		virtual bool TryLock(LockMode mode) = 0;
		virtual void Unlock()				= 0;
	};

	class ReadWriteLock final
	{
	  public:
		ReadWriteLock();
		virtual ~ReadWriteLock() = default;
		void Lock(LockMode mode);
		bool TryLock(LockMode mode);
		void Unlock();

	  private:
		std::unique_ptr<ReadWriteLockBase> m_BaseReadWriteLock = nullptr;
	};
}	 // namespace Nexus::Threading