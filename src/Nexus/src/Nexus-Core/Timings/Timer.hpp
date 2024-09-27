#pragma once

#include "Clock.hpp"
#include "Timespan.hpp"
#include "nxpch.hpp"

namespace Nexus::Timings
{
	class ExecutionTimer
	{
	  public:
		enum class ExecutionPolicy
		{
			Every,
			After
		};

	  private:
		struct FunctionToExecute
		{
			std::function<void(Nexus::TimeSpan)> Func		   = {};
			std::optional<double>				 WhenToExecute = 0;
			Nexus::TimeSpan						 Timer		   = {};
			ExecutionPolicy						 Policy		   = {};
		};

	  public:
		inline void Update()
		{
			m_Clock.Tick();
			TryExecuteFunctions();
		}

		inline void Clear()
		{
			m_Functions.clear();
		}

		inline void After(std::function<void(Nexus::TimeSpan)> function, std::optional<double> seconds)
		{
			AddFunction(function, seconds, ExecutionPolicy::After);
		}

		inline void Every(std::function<void(Nexus::TimeSpan)> function, std::optional<double> seconds)
		{
			AddFunction(function, seconds, ExecutionPolicy::Every);
		}

	  private:
		void AddFunction(std::function<void(Nexus::TimeSpan)> func, std::optional<double> seconds, ExecutionPolicy policy)
		{
			FunctionToExecute funcToExecute = {.Func = func, .WhenToExecute = seconds, .Timer = Nexus::TimeSpan(0), .Policy = policy};
			m_Functions.push_back(funcToExecute);
		}

		void TryExecuteFunctions()
		{
			TimeSpan time = m_Clock.GetTime();

			for (size_t index = 0; index < m_Functions.size(); index++)
			{
				FunctionToExecute &func = m_Functions[index];
				func.Timer += time;

				// if no time has been provided for when to execute
				if (!func.WhenToExecute.has_value())
				{
					func.Func(func.Timer);
					func.Timer = {};
					return;
				}

				// execute a timed query
				if (func.Timer.GetSeconds() >= func.WhenToExecute.value())
				{
					func.Func(func.Timer);

					if (func.Policy == ExecutionPolicy::After)
					{
						m_Functions.erase(m_Functions.begin() + index);
						index--;
					}
					else
					{
						func.Timer = {};
					}
				}
			}
		}

	  private:
		Clock						   m_Clock	   = {};
		std::vector<FunctionToExecute> m_Functions = {};
	};

	class ProfilingTimer
	{
	  public:
		ProfilingTimer(const char *name) : m_Name(name), m_StartTimepoint(std::chrono::high_resolution_clock::now()), m_Stopped(false)
		{
		}

		~ProfilingTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			std::chrono::steady_clock::time_point endTimepoint = std::chrono::high_resolution_clock::now();
			uint64_t elapsed	= std::chrono::duration_cast<std::chrono::nanoseconds>(m_StartTimepoint - endTimepoint).count();
			m_Stopped			= true;
			double milliseconds = (double)elapsed / 1000000000.0;

			std::cout << "Duration: " << std::fixed << std::setprecision(3) << milliseconds << std::endl;
		}

	  private:
		const char										  *m_Name			= {};
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint = std::chrono::high_resolution_clock::now();
		bool											   m_Stopped		= false;
	};
}	 // namespace Nexus::Timings