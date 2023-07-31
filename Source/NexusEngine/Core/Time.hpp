#pragma once

namespace Nexus
{
    /// @brief A class representing a time
    class Time
    {
    public:
        /// @brief A default constructor that creates a time set to zero
        Time() = default;

        /// @brief A constructor that takes in nanoseconds to represent
        /// @param nanoseconds A double representing the number of nanoseconds to represent
        Time(double nanoseconds)
        {
            m_Nanoseconds = nanoseconds;
        }

        /// @brief A method to return the number of nanoseconds
        /// @return A double representing the number of nanoseconds
        double GetNanoseconds()
        {
            return m_Nanoseconds;
        }

        /// @brief A method to return the number of milliseconds
        /// @return A double representing the number of milliseconds
        double GetMilliseconds()
        {
            return m_Nanoseconds / 1000000;
        }

        /// @brief A method to return the number of seconds
        /// @return A double representing the number of seconds
        double GetSeconds()
        {
            return m_Nanoseconds / 1000000000;
        }

    private:
        /// @brief A double value representing the number of nanoseconds
        double m_Nanoseconds = 0;
    };
}