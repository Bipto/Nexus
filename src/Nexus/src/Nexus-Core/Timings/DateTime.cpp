#include "Nexus-Core/Timings/DateTime.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus
{
	DateTime DateTime::FromNanoseconds(uint64_t nanoseconds)
	{
		DateTime out = {};
		Utils::ConvertNanosecondsToTm(nanoseconds, out.m_Timestamp);
		return out;
	}

	DateTime DateTime::Now()
	{
		auto now		 = std::chrono::system_clock::now();
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
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
		return (DayOfWeek)m_Timestamp.tm_wday;
	}

	uint8_t DateTime::GetDayOfMonth() const
	{
		return m_Timestamp.tm_mday;
	}

	uint8_t DateTime::GetDayOfYear() const
	{
		return m_Timestamp.tm_yday;
	}

	Month DateTime::GetMonth() const
	{
		return (Month)m_Timestamp.tm_mon;
	}

	uint16_t DateTime::GetYear() const
	{
		return m_Timestamp.tm_year;
	}

}	 // namespace Nexus
