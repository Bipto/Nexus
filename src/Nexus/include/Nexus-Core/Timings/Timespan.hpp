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

		static TimeSpan FromMilliseconds(uint64_t milliseconds)
		{
			TimeSpan timeSpan	   = {};
			timeSpan.m_Nanoseconds = milliseconds * 1'000'000;
			return timeSpan;
		}

		static TimeSpan FromSeconds(uint64_t seconds)
		{
			TimeSpan timeSpan	   = {};
			timeSpan.m_Nanoseconds = seconds * 1'000'000'000;
			return timeSpan;
		}

		template<typename T>
		T GetNanoseconds() const
		{
			return (T)m_Nanoseconds;
		}

		template<typename T>
		T GetMilliseconds() const
		{
			return (T)(GetNanoseconds<T>() / 1'000'000);
		}

		template<typename T>
		T GetSeconds() const
		{
			return (T)(GetMilliseconds<T>() / 1000);
		}

		template<typename T>
		T GetMinutes() const
		{
			return (T)(GetSeconds<T>() / 60);
		}

		template<typename T>
		T GetHours() const
		{
			return (T)(GetMinutes<T>() / 60);
		}

		TimeSpan &operator+=(const TimeSpan &other)
		{
			m_Nanoseconds += other.m_Nanoseconds;
			return *this;
		}

	  private:
		/// @brief An unsigned integer value representing the number of nanoseconds
		uint64_t m_Nanoseconds = 0;
	};
}	 // namespace Nexus