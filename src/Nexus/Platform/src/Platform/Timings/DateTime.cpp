
#include <ctime>

#include "Core/Utils.hpp"
#include "Platform/Timings/DateTime.hpp"

namespace
{
    void ConvertNanosecondsToTm(uint64_t nanoseconds, std::tm &outTime)
    {
        std::time_t seconds = nanoseconds / 1'000'000'000;

#if defined(_WIN32)
        // Windows secure version
        gmtime_s(&outTime, &seconds);
#else
        // POSIX thread-safe version
        gmtime_r(&seconds, &outTime);
#endif
    }
} // namespace

namespace Nexus
{
    DateTime DateTime::FromNanoseconds(uint64_t nanoseconds)
    {
        DateTime out = {};
        ConvertNanosecondsToTm(nanoseconds, out.m_Timestamp);
        return out;
    }

    DateTime DateTime::Now()
    {
        auto now = std::chrono::system_clock::now();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
                               now.time_since_epoch()
        )
                               .count();
        return DateTime::FromNanoseconds(nanoseconds);
    }

    uint8_t DateTime::GetSecond() const
    {
        return m_Timestamp.tm_sec;
    }

    uint8_t DateTime::GetMinute() const
    {
        return m_Timestamp.tm_min;
    }

    uint8_t DateTime::GetHour() const
    {
        return m_Timestamp.tm_hour;
    }

    DayOfWeek DateTime::GetDayOfWeek() const
    {
        return static_cast<DayOfWeek>(m_Timestamp.tm_wday + 1);
    }

    uint8_t DateTime::GetDayOfMonth() const
    {
        return m_Timestamp.tm_mday;
    }

    uint16_t DateTime::GetDayOfYear() const
    {
        return m_Timestamp.tm_yday;
    }

    Month DateTime::GetMonth() const
    {
        return static_cast<Month>(m_Timestamp.tm_mon + 1);
    }

    uint16_t DateTime::GetYear() const
    {
        // offset from epoch
        return m_Timestamp.tm_year + 1900;
    }

    bool DateTime::IsDaylightSavingsTime() const
    {
        return m_Timestamp.tm_isdst > 0;
    }
} // namespace Nexus
