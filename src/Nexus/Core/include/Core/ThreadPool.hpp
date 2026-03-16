#pragma once

#include "NamedJThread.hpp"

#include <atomic>
#include <condition_variable>
#include <format>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Nexus
{
	/// @brief Class representing a set of threads and tasks that can be submitted to them
	class ThreadPool
	{
	  public:
		/// @brief Constructor for ThreadPool, taking in the name, number of threads and maximum number of queued functions
		/// @param name The name of the thread pool for debugging, name will be applied to all created threads
		/// @param threadCount The number of threads to use in the thread pool
		/// @param maxQueueSize The maximum number of queue functions to call
		explicit ThreadPool(std::string_view name, size_t threadCount, size_t maxQueueSize = 1024);

		/// @brief A destructor to clean up the thread pool
		~ThreadPool();

		/// @brief A function to submit work into one of the threads in the pool
		/// @param job The function to call on the thread
		void Submit(std::function<void()> job);

		/// @brief A function that submits work into one of the threads in the pool
		/// @tparam F The type of the function to call
		/// @tparam ...Args The Type of the arguments to pass to the function
		/// @param f The actual function to call
		/// @param ...args The parameters to pass into the function
		/// @return The value returned from the function called by the thread
		template<typename F, typename... Args>
		auto Submit(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>;

		/// @brief A function that prevents new submissions and helps to exit all threads
		void Stop();

		/// @brief A function that requests that all threads in the pool should join
		void Join();

		/// @brief A function that waits until all work in the pool has been completed
		void WaitUntilIdle();

		/// @brief A function that temporarily pauses all execution within the thread pool
		void Pause();

		/// @brief A function that resumes execution within the thread pool
		void Resume();

	  private:
		/// @brief A function that is internally called by the thread pool to execute queued functions
		void WorkerLoop();

	  private:
		/// @brief The debug name of the thread pool
		std::string m_Name {};

		/// @brief A vector of all worker threads within the thread pool
		std::vector<std::unique_ptr<NamedJThread>> m_Workers {};

		/// @brief A queue of all jobs to be executed
		std::queue<std::function<void()>> m_Jobs {};

		/// @brief A mutex to handle synchronisation between threads
		std::mutex m_Mutex {};

		/// @brief A condition variable to synchronise sharing of work between threads
		std::condition_variable m_Condition {};

		/// @brief A condition variable to check when the thread pool is idle
		std::condition_variable m_IdleCondition {};

		/// @brief An integer containing the number of jobs currently being executed by the thread pool
		std::atomic<size_t> m_ActiveJobs {0};

		/// @brief The maximum number of threads that can be queued by the thread pool
		size_t m_MaxQueueSize {1024};

		/// @brief A boolean indicating whether the thread pool is stopping
		bool m_Stop {false};

		/// @brief a boolean indicating whether the thread pool is currently paused
		bool m_Paused {false};
	};

}	 // namespace Nexus

#include "ThreadPool.inl"