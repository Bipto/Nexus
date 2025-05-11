#include "Nexus-Core/Timings/DateTime.hpp"

int main()
{
	Nexus::DateTime	 now		= Nexus::DateTime::Now();
	Nexus::DayOfWeek day		= now.GetDayOfWeek();
	uint8_t			 dayOfMonth = now.GetDayOfMonth();
	Nexus::Month	 month		= now.GetMonth();
	uint8_t			 hour		= now.GetHour();
	uint8_t			 minute		= now.GetMinute();
}