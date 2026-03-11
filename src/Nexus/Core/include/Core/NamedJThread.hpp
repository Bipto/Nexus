#pragma once

#include <chrono>
#include <functional>
#include <latch>
#include <memory>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#if defined(__linux__) || defined(__APPLE__)
	#include <pthread.h>
#endif

#if defined(_WIN32)
	#include <windows.h>
#endif

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
	class NamedJThread
	{
	  public:
		template<typename Callable, typename... Args>
		NamedJThread(std::string_view name, Callable &&func, Args &&...args) : m_Name(name)
		{
			m_Thread = std::jthread(
				[this, func = std::forward<Callable>(func), ... args = std::forward<Args>(args)](std::stop_token st) mutable
				{
					m_StartTime = std::chrono::steady_clock::now();
					SetCurrentThreadName(m_Name);

					m_Running = true;
					if (m_OnStart)
					{
						m_OnStart();
					}

					m_Started.count_down();

					try
					{
						std::invoke(func, st, args...);
					}
					catch (...)
					{
						if (m_OnException)
							m_OnException(std::current_exception());
						throw;
					}

					m_Running = false;
					m_Stopped.count_down();

					if (m_OnStop)
					{
						m_OnStop();
					}
				});
		}

		// Movable, not copyable
		NamedJThread(NamedJThread &&) noexcept			  = default;
		NamedJThread &operator=(NamedJThread &&) noexcept = default;

		NamedJThread(const NamedJThread &)			  = delete;
		NamedJThread &operator=(const NamedJThread &) = delete;

		// Thread API passthrough
		void Join()
		{
			m_Thread.join();
			if (m_Exception)
				std::rethrow_exception(m_Exception);
		}

		void Detach()
		{
			m_Thread.detach();
		}

		bool Joinable() const
		{
			return m_Thread.joinable();
		}

		std::thread::id GetID() const
		{
			return m_Thread.get_id();
		}

		std::string_view GetName() const
		{
			return m_Name;
		}

		void RequestStop()
		{
			m_Thread.request_stop();
		}

		void WaitUntilStarted()
		{
			m_Started.wait();
		}

		void WaitUntilStopped()
		{
			m_Stopped.wait();
		}

		bool StopRequested() const
		{
			return m_Thread.get_stop_token().stop_requested();
		}

		std::stop_token GetStopToken() const
		{
			return m_Thread.get_stop_token();
		}

		void SetOnStart(std::function<void()> func)
		{
			m_OnStart = std::move(func);
		}

		void SetOnStop(std::function<void()> func)
		{
			m_OnStop = std::move(func);
		}

		void SetOnException(std::function<void(std::exception_ptr)> func)
		{
			m_OnException = std::move(func);
		}

		auto Uptime() const
		{
			return std::chrono::steady_clock::now() - m_StartTime;
		}

		std::string Describe() const
		{
			return "Thread '" + m_Name + "' id=" + std::to_string(std::hash<std::thread::id> {}(m_Thread.get_id())) +
				   " running=" + (m_Running ? "true" : "false") +
				   " uptime=" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Uptime()).count()) + "ms";
		}

		bool IsRunning() const
		{
			return m_Running;
		}

	  private:
		std::string								m_Name = {};
		std::latch								m_Started {1};
		std::latch								m_Stopped {1};
		std::atomic<bool>						m_Running	= false;
		std::exception_ptr						m_Exception = {};
		std::chrono::steady_clock::time_point	m_StartTime;
		std::jthread							m_Thread	  = {};
		std::function<void()>					m_OnStart	  = {};
		std::function<void()>					m_OnStop	  = {};
		std::function<void(std::exception_ptr)> m_OnException = {};
	};
}	 // namespace Nexus