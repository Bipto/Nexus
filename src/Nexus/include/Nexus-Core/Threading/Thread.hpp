#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	enum class ThreadPriority
	{
		Default,
		Low,
		Normal,
		High,
		TimeCritical
	};

	enum class ThreadState
	{
		Uknown,
		Alive,
		Detached,
		Complete
	};

	struct ThreadDescription
	{
		std::string Name	  = "Thread";
		uint64_t	StackSize = 0;
		ThreadPriority Priority	 = ThreadPriority::Default;
	};

	class NX_API ThreadBase
	{
	  public:
		virtual ~ThreadBase()									= default;
		virtual const ThreadDescription &GetDescription() const = 0;
		virtual ThreadState				 GetThreadState() const = 0;
		virtual void					 Wait() const;
		virtual void					 Detach() const;

	  private:
		std::function<void()> m_Function;
	};

	class NX_API Thread
	{
	  public:
		template<typename Func, typename... Args>
		explicit Thread(const ThreadDescription &description, Func &&f, Args &&...args)
		{
			auto boundFunc = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
			m_Function	   = std::move(boundFunc);
			CreateBase(description);
		}

		virtual ~Thread() = default;
		const ThreadDescription &GetDescription() const;
		ThreadState				 GetThreadState() const;
		void					 Wait() const;
		void					 Detach() const;

	  private:
		void CreateBase(const ThreadDescription &description);

	  private:
		std::unique_ptr<ThreadBase> m_ThreadBase = nullptr;
		std::function<void()>		m_Function;
	};
}	 // namespace Nexus