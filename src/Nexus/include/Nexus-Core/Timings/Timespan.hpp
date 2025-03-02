#pragma once

namespace Nexus
{
	/// @brief A class representing a time
	class TimeSpan
	{
	  public:
		/// @brief A default constructor that creates a time set to zero
		TimeSpan() = default;

		/// @brief A constructor that takes in nanoseconds to represent
		/// @param nanoseconds A double representing the number of nanoseconds to
		/// represent
		explicit TimeSpan(double nanoseconds)
		{
			m_Nanoseconds = nanoseconds;
		}

		/// @brief A method to return the number of nanoseconds
		/// @return A double representing the number of nanoseconds
		double GetNanoseconds() const
		{
			return m_Nanoseconds;
		}

		/// @brief A method to return the number of milliseconds
		/// @return A double representing the number of milliseconds
		double GetMilliseconds() const
		{
			return GetNanoseconds() / 1000000;
		}

		/// @brief A method to return the number of seconds
		/// @return A double representing the number of seconds
		double GetSeconds() const
		{
			return GetMilliseconds() / 1000;
		}

		double GetMinutes() const
		{
			return GetSeconds() / 60;
		}

		double GetHours() const
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
		double m_Nanoseconds = 0;
	};
}	 // namespace Nexus