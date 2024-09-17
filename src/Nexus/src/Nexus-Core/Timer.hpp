#pragma once

#include "Clock.hpp"
#include "Time.hpp"
#include "nxpch.hpp"

namespace Nexus
{
	class Timer
	{
	  public:
		inline void Update()
		{
			m_Clock.Tick();
			TimeSpan time = m_Clock.GetTime();

			for (size_t index = 0; index < m_Functions.size(); index++)
			{
				FunctionToExecute &func = m_Functions[index];
				func.Timer += time;

				if (func.Timer.GetSeconds() >= func.WhenToExecute)
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

		inline void After(std::function<void(Nexus::TimeSpan)> function, double seconds)
		{
			AddFunction(function, seconds, ExecutionPolicy::After);
		}

		inline void Every(std::function<void(Nexus::TimeSpan)> function, double seconds)
		{
			AddFunction(function, seconds, ExecutionPolicy::Every);
		}

	  private:
		enum class ExecutionPolicy
		{
			Every,
			After
		};

		struct FunctionToExecute
		{
			std::function<void(Nexus::TimeSpan)> Func		   = {};
			double								 WhenToExecute = 0;
			Nexus::TimeSpan						 Timer		   = {};
			Timer::ExecutionPolicy				 Policy		   = {};
		};

	  private:
		void AddFunction(std::function<void(Nexus::TimeSpan)> func, double seconds, Timer::ExecutionPolicy policy)
		{
			FunctionToExecute funcToExecute = {.Func = func, .WhenToExecute = seconds, .Timer = Nexus::TimeSpan(0), .Policy = policy};
			m_Functions.push_back(funcToExecute);
		}

	  private:
		Clock						   m_Clock	   = {};
		std::vector<FunctionToExecute> m_Functions = {};
	};
}	 // namespace Nexus