#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	enum class DayOfWeek
	{
		Sunday	  = 0,
		Monday	  = 1,
		Tuesday	  = 2,
		Wednesday = 3,
		Thursday  = 4,
		Friday	  = 5,
		Saturday  = 6
	};

	enum class Month
	{
		January	  = 0,
		February  = 1,
		March	  = 2,
		April	  = 3,
		May		  = 4,
		June	  = 5,
		July	  = 6,
		August	  = 7,
		September = 8,
		October	  = 9,
		November  = 10,
		December  = 11
	};

	class NX_API DateTime
	{
	  public:
		DateTime() = default;
		static DateTime FromNanoseconds(uint64_t nanoseconds);
		static DateTime Now();

		uint8_t	  GetSecond() const;
		uint8_t	  GetMinute() const;
		uint8_t	  GetHour() const;
		DayOfWeek GetDayOfWeek() const;
		uint8_t	  GetDayOfMonth() const;
		uint8_t	  GetDayOfYear() const;
		Month	  GetMonth() const;
		uint16_t  GetYear() const;
		bool	  IsDaylightSavingsTime() const;

	  private:
		std::tm m_Timestamp = {};
	};
}	 // namespace Nexus