#pragma once

#if defined(__linux__) || defined(__APPLE__)
	#include <pthread.h>
#endif

#if defined(_WIN32)
	#include <windows.h>
#endif

#include "NamedJThread.hpp"
#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <latch>
#include <memory>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace
{
	inline void SetCurrentThreadName(const std::string &name)
	{
#if defined(__linux__)
		pthread_setname_np(pthread_self(), name.c_str());
#elif defined(__APPLE__)
		pthread_setname_np(name.c_str());
#elif defined(_WIN32)
		SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
#endif
	}
}	 // namespace

namespace Nexus
{
	template<typename Callable, typename... Args>
	inline NamedJThread::NamedJThread(std::string_view						  name,
									  std::function<void()>					  onStart,
									  std::function<void()>					  onStop,
									  std::function<void(std::exception_ptr)> onException,
									  Callable								&&func,
									  Args &&...args)
		: m_Name(name),
		  m_OnStart(std::move(onStart)),
		  m_OnStop(std::move(onStop)),
		  m_OnException(std::move(onException))
	{
		m_Thread = std::jthread(
			[this, func = std::forward<Callable>(func), ... args = std::forward<Args>(args)](std::stop_token st) mutable
			{
				m_StartTime = std::chrono::steady_clock::now();
				SetCurrentThreadName(m_Name);

				m_Running = true;
				if (m_OnStart)
					m_OnStart();

				m_Started.count_down();

				try
				{
					std::invoke(func, st, args...);
				}
				catch (...)
				{
					m_Exception = std::current_exception();

					if (m_OnException)
						m_OnException(m_Exception);
				}

				m_Running = false;
				m_Stopped.count_down();

				if (m_OnStop)
					m_OnStop();
			});
	}

	inline void NamedJThread::Join()
	{
		m_Thread.join();
		if (m_Exception)
			std::rethrow_exception(m_Exception);
	}

	inline void NamedJThread::Detach()
	{
		m_Thread.detach();
	}

	inline bool NamedJThread::Joinable() const
	{
		return m_Thread.joinable();
	}

	inline std::thread::id NamedJThread::GetID() const
	{
		return m_Thread.get_id();
	}

	inline std::string_view NamedJThread::GetName() const
	{
		return m_Name;
	}

	inline void NamedJThread::RequestStop()
	{
		m_Thread.request_stop();
	}

	inline void NamedJThread::WaitUntilStarted() const
	{
		m_Started.wait();
	}

	inline void NamedJThread::WaitUntilStopped() const
	{
		m_Stopped.wait();
	}

	inline bool NamedJThread::StopRequested() const
	{
		return m_Thread.get_stop_token().stop_requested();
	}

	inline std::stop_token NamedJThread::GetStopToken() const
	{
		return m_Thread.get_stop_token();
	}

	inline auto NamedJThread::Uptime() const
	{
		return std::chrono::steady_clock::now() - m_StartTime;
	}

	inline std::string NamedJThread::Describe() const
	{
		return "Thread '" + m_Name + "' id=" + std::to_string(std::hash<std::thread::id> {}(m_Thread.get_id())) +
			   " running=" + (m_Running ? "true" : "false") +
			   " uptime=" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Uptime()).count()) + "ms";
	}

	inline bool NamedJThread::IsRunning() const
	{
		return m_Running;
	}
}	 // namespace Nexus