#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A class representing a time
	class TimeSpan
	{
	  public:
		/// @brief A default constructor that creates a time set to zero
		TimeSpan() = default;

		static TimeSpan FromNanoseconds(uint64_t nanoseconds)
		{
			TimeSpan timeSpan	   = {};
			timeSpan.m_Nanoseconds = nanoseconds;
			return timeSpan;
		}

		/// @brief A method to return the number of nanoseconds
		/// @return A double representing the number of nanoseconds
		uint64_t GetNanoseconds() const
		{
			return m_Nanoseconds;
		}

		/// @brief A method to return the number of milliseconds
		/// @return A double representing the number of milliseconds
		uint64_t GetMilliseconds() const
		{
			return GetNanoseconds() / 1000000;
		}

		/// @brief A method to return the number of seconds
		/// @return A double representing the number of seconds
		uint64_t GetSeconds() const
		{
			return GetMilliseconds() / 1000;
		}

		uint64_t GetMinutes() const
		{
			return GetSeconds() / 60;
		}

		uint64_t GetHours() const
		{
			return GetMinutes() / 60;
		}

		TimeSpan &operator+=(const TimeSpan &other)
		{
			m_Nanoseconds += other.m_Nanoseconds;
			return *this;
		}

	  private:
		/// @brief A double value representing the number of nanoseconds
		uint64_t m_Nanoseconds = 0;
	};
}	 // namespace Nexus