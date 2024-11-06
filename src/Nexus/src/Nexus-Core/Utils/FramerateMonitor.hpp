#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
	class FrameRateMonitor
	{
	  public:
		inline void Update()
		{
			std::chrono::steady_clock::time_point	  now	  = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::milli> elapsed = now - m_LastFrameTime;
			m_LastFrameTime									  = now;

			m_Elapsed = elapsed.count();
		}

		inline double GetMillisecondsPerFrame() const
		{
			return m_Elapsed;
		}

		inline double GetSecondsPerFrame() const
		{
			return m_Elapsed / 1000.0;
		}

		inline double GetFPS() const
		{
			return 1 / GetSecondsPerFrame();
		}

	  private:
		std::chrono::steady_clock::time_point m_LastFrameTime = std::chrono::steady_clock::now();
		double								  m_Elapsed		  = 0.0;
	};
}	 // namespace Nexus::Utils