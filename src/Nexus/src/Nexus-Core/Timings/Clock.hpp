#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Timespan.hpp"

namespace Nexus
{
	/// @brief A class represenging a clock to use for timings
	class Clock
	{
	  public:
		Clock() = default;

		/// @brief A method allowing the clock to be updating to the currently elapsed
		/// time
		inline void Tick()
		{
			std::chrono::high_resolution_clock::time_point tickTime = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(tickTime - m_StartTime).count();
			m_StartTime = tickTime;
		}

		/// @brief A method allowing the clock to return a structure representing the
		/// elapsed time
		/// @return A Nexus::Time object containing the currently elapsed time
		inline Nexus::TimeSpan GetTime()
		{
			return Nexus::TimeSpan(m_DeltaTime);
		}

	  private:
		/// @brief A variable containing the time that the clock was created
		std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();

		/// @brief A delta time representing how much time has elapsed since the clock
		/// was created
		uint64_t m_DeltaTime {};
	};
}	 // namespace Nexus