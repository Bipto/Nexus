#pragma once

#include <chrono>
#include <cstdint>

#include "Platform/Platform-Core.hpp"

namespace Nexus
{
    enum class DayOfWeek
    {
        Sunday = 1,
        Monday = 2,
        Tuesday = 3,
        Wednesday = 4,
        Thursday = 5,
        Friday = 6,
        Saturday = 7
    };

    enum class Month
    {
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12
    };

    class NX_PLATFORM_API DateTime
    {
      public:
        DateTime() = default;
        static DateTime FromNanoseconds(uint64_t nanoseconds);
        static DateTime Now();

        uint8_t GetSecond() const;
        uint8_t GetMinute() const;
        uint8_t GetHour() const;
        DayOfWeek GetDayOfWeek() const;
        uint8_t GetDayOfMonth() const;
        uint16_t GetDayOfYear() const;
        Month GetMonth() const;
        uint16_t GetYear() const;
        bool IsDaylightSavingsTime() const;

      private:
        std::tm m_Timestamp = {};
    };
} // namespace Nexus