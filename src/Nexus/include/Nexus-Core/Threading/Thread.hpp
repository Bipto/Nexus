#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	struct ThreadDescription
	{
		std::string Name	  = "Thread";
		uint64_t	StackSize = 0;
	};

	class NX_API ThreadBase
	{
	  public:
		virtual ~ThreadBase()									= default;
		virtual const ThreadDescription &GetDescription() const = 0;

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

		virtual ~Thread()
		{
		}

	  private:
		inline void CreateBase(const ThreadDescription &description);

	  private:
		std::unique_ptr<ThreadBase> m_ThreadBase = nullptr;
		std::function<void()>		m_Function;
	};
}	 // namespace Nexus