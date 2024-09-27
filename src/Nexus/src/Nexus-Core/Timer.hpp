#pragma once

#include "Clock.hpp"
#include "Time.hpp"
#include "nxpch.hpp"

namespace Nexus::Timings
{
	enum class ExecutionPolicy
	{
		Every,
		After
	};

	struct FunctionToExecute
	{
		std::function<void(Nexus::TimeSpan)> Func		   = {};
		std::optional<double>				 WhenToExecute = 0;
		Nexus::TimeSpan						 Timer		   = {};
		ExecutionPolicy						 Policy		   = {};
	};

	class Timer
	{
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

				if (!func.WhenToExecute.has_value())
				{
					func.Func(func.Timer);
					func.Timer = {};
					return;
				}

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
}	 // namespace Nexus