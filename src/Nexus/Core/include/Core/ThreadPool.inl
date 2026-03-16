#pragma once

#include "NamedJThread.hpp"
#include "ThreadPool.hpp"

#include <atomic>
#include <condition_variable>
#include <format>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

namespace Nexus
{
	inline ThreadPool::ThreadPool(std::string_view name, size_t threadCount, size_t maxQueueSize) : m_Name(name), m_MaxQueueSize(maxQueueSize)
	{
		m_Workers.reserve(threadCount);

		for (size_t i = 0; i < threadCount; ++i)
		{
			std::string workerName = std::format("{} thread-{}", m_Name, i);
			m_Workers.emplace_back(std::make_unique<NamedJThread>(workerName, nullptr, nullptr, nullptr, [this](std::stop_token) { WorkerLoop(); }));
		}

		for (auto &worker : m_Workers) worker->WaitUntilStarted();
	}

	inline ThreadPool::~ThreadPool()
	{
		Stop();
		Join();
	}

	// Submit a void job
	inline void ThreadPool::Submit(std::function<void()> job)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);

		// Wake when either stopped OR there is space in the queue
		m_Condition.wait(lock, [&] { return m_Stop || m_Jobs.size() < m_MaxQueueSize; });

		if (m_Stop)
			throw std::runtime_error("ThreadPool: Submit() called after Stop()");

		m_Jobs.push(std::move(job));
		m_Condition.notify_one();
	}

	// Submit a job that returns a value
	template<typename F, typename... Args>
	inline auto ThreadPool::Submit(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>
	{
		using ReturnT = std::invoke_result_t<F, Args...>;

		auto task = std::make_shared<std::packaged_task<ReturnT()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<ReturnT> result = task->get_future();

		{
			std::unique_lock<std::mutex> lock(m_Mutex);

			m_Condition.wait(lock, [&] { return m_Stop || m_Jobs.size() < m_MaxQueueSize; });

			if (m_Stop)
				throw std::runtime_error("ThreadPool: Submit() called after Stop()");

			m_Jobs.push([task]() { (*task)(); });
		}

		m_Condition.notify_one();
		return result;
	}

	inline void ThreadPool::Stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Stop = true;
		}

		// Wake all waiting submitters and workers
		m_Condition.notify_all();
		m_IdleCondition.notify_all();
	}

	inline void ThreadPool::Join()
	{
		for (auto &worker : m_Workers)
		{
			if (worker->Joinable())
				worker->Join();
		}
	}

	inline void ThreadPool::WaitUntilIdle()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_IdleCondition.wait(lock, [&] { return m_Jobs.empty() && m_ActiveJobs.load() == 0; });
	}

	inline void ThreadPool::Pause()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Paused = true;
	}

	inline void ThreadPool::Resume()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Paused = false;
		}
		m_Condition.notify_all();
	}

	inline void ThreadPool::WorkerLoop()
	{
		for (;;)
		{
			std::function<void()> job;

			{
				std::unique_lock<std::mutex> lock(m_Mutex);

				// Wake when:
				//  - stop requested (m_Stop), or
				//  - not paused and there is work
				m_Condition.wait(lock, [&] { return m_Stop || (!m_Paused && !m_Jobs.empty()); });

				// If stopping and no work left, exit
				if (m_Stop && m_Jobs.empty())
					return;

				// If paused or no jobs (spurious wake), loop again
				if (m_Paused || m_Jobs.empty())
					continue;

				job = std::move(m_Jobs.front());
				m_Jobs.pop();
				m_ActiveJobs.fetch_add(1, std::memory_order_relaxed);

				// we just freed a slot in the queue -> wake blocked submitters
				m_Condition.notify_all();
			}

			if (job)
				job();

			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_ActiveJobs.fetch_sub(1, std::memory_order_relaxed);

				if (m_Jobs.empty() && m_ActiveJobs.load(std::memory_order_relaxed) == 0)
					m_IdleCondition.notify_all();
			}
		}
	}
}	 // namespace Nexus