#pragma once

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

namespace Nexus
{
    /// @brief Class representing a thread that can be named and will automatically
    /// join when being destructed
    class NamedJThread
    {
      public:
        /// @brief Constructor to create a thread, taking in a name, the function to
        /// run on the new thread, parameters to pass into it and optionally
        /// functions to call when the thread starts, stops and encounters an
        /// exception
        /// @tparam Callable The type of the function to be executed by the thread
        /// @tparam ...Args The types of the parameters to pass into Callable
        /// @param name The name to be assigned to the thread
        /// @param onStart The function to call when the thread is created, but
        /// before it begins executing the supplied function
        /// @param onStop The function to call when the thread has finished executing
        /// the supplied function and is terminating
        /// @param onException The function to call when the thread encounters an
        /// exception
        /// @param func The function to call from the new thread
        /// @param ...args The arguments that will be forwarded into Callable
        /// @param
        template <typename Callable, typename... Args>
        NamedJThread(
            std::string_view name, std::function<void()> onStart,
            std::function<void()> onStop,
            std::function<void(std::exception_ptr)> onException, Callable &&func,
            Args &&...args
        );

        /// @brief Move constructor
        /// @param The other object to move into this instance
        NamedJThread(NamedJThread &&) noexcept = default;

        /// @brief Move assignment operator
        /// @param An r-value reference to the other instance to move into this
        /// object
        /// @return A reference to the current instance
        NamedJThread &operator=(NamedJThread &&) noexcept = default;

        /// @brief The copy constructor is not supported by this class
        /// @param A reference to thread to copy
        NamedJThread(const NamedJThread &) = delete;

        /// @brief The copy assignment operator is not supported by this class
        /// @param  A reference to the thread to copy
        /// @return A reference to the newly constructed object
        NamedJThread &operator=(const NamedJThread &) = delete;

        /// @brief A function which waits for the thread to finish executing
        void Join();

        /// @brief A function which permits execution to continue independently of
        /// the thread handle
        void Detach();

        /// @brief A function that checks whether the thread is joinable
        /// @return A boolean indicating whether the thread is able to be joined
        bool Joinable() const;

        /// @brief A function that returns the ID of the thread
        /// @return The ID of the thread
        std::thread::id GetID() const;

        /// @brief A function that returns the name of the thread
        /// @return A string_view containing the thread name
        std::string_view GetName() const;

        /// @brief A function that requests that the thread finishes executing
        void RequestStop();

        /// @brief A function that will cause the callee to wait until this thread
        /// begins executing the requested function
        void WaitUntilStarted() const;

        /// @brief A function that will cause the callee to wait until this thread
        /// has finished executing the requested function
        void WaitUntilStopped() const;

        /// @brief A function that checks whether the thread has been requested to
        /// terminate
        /// @return A boolean indicating whether the thread is terminating
        bool StopRequested() const;

        /// @brief A function that returns the stop token associated with the state
        /// of the thread
        /// @return A stop_token representing the shared stop state of the thread
        std::stop_token GetStopToken() const;

        /// @brief A function that checks how long the thread has been running for
        /// @return The elapsed time since the thread began executing
        auto Uptime() const;

        /// @brief A helper function that describes the current state of the thread
        /// @return A string containing the thread's description
        std::string Describe() const;

        /// @brief A function that checks whether the thread is currently executing
        /// @return A boolean indicating whether the thread is currently running
        bool IsRunning() const;

      private:
        /// @brief A string containing the debug name of the thread
        std::string m_Name;

        /// @brief A latch to identify when the thread has registered as starting
        std::latch m_Started{1};

        /// @brief A latch to identify when the thread has registered as stopping
        std::latch m_Stopped{1};

        /// @brief A boolean indicating whether the thread is registered as currently
        /// running
        std::atomic<bool> m_Running = false;

        /// @brief An exception pointer containing the currently encountered
        /// exception (if exists)
        std::exception_ptr m_Exception = nullptr;

        /// @brief A timestamp of when the thread began executing
        std::chrono::steady_clock::time_point m_StartTime;

        /// @brief The underlying std::jthread being wrapped
        std::jthread m_Thread;

        /// @brief A function pointer to a function to call when starting execution
        /// on the thread
        std::function<void()> m_OnStart;

        /// @brief A function pointer to a function to call when stopping execution
        /// on the thread
        std::function<void()> m_OnStop;

        /// @brief A function pointer to a function to call when an exception is
        /// encountered on the thread
        std::function<void(std::exception_ptr)> m_OnException;
    };
} // namespace Nexus

#include "NamedJThread.inl"