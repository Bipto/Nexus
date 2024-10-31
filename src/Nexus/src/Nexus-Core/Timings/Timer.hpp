#pragma once

#include "Clock.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Timespan.hpp"

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
		ProfilingTimer(const char *name, const std::source_location location = std::source_location::current())
			: m_Name(name),
			  m_StartTimepoint(std::chrono::steady_clock::now()),
			  m_Stopped(false),
			  m_Location(location)
		{
		}

		~ProfilingTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			std::chrono::steady_clock::time_point endTimepoint = std::chrono::steady_clock::now();

			uint64_t start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimepoint).time_since_epoch().count();
			uint64_t end   = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimepoint).time_since_epoch().count();

			uint64_t elapsed = end - start;
			m_Stopped		 = true;
			OnStop.Invoke(TimeSpan(elapsed));
		}

		const char *GetName() const
		{
			return m_Name;
		}

		const std::source_location &GetSourceLocation() const
		{
			return m_Location;
		}

		std::string GenerateName() const
		{
			std::stringstream ss;
			ss << m_Name << ": ";
			ss << m_Location.function_name() << " ";
			ss << m_Location.line() << ", " << m_Location.column();

			return ss.str();
		}

	  public:
		EventHandler<TimeSpan> OnStop;

	  private:
		const char										  *m_Name			= {};
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint = std::chrono::steady_clock::now();
		bool											   m_Stopped		= false;
		std::source_location							   m_Location		= {};
	};
}	 // namespace Nexus::Timings