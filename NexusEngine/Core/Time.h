#pragma once

namespace Nexus
{
    class Time
    {
        public:
            Time() = default;
            Time(double nanoseconds)
            {
                m_Nanoseconds = nanoseconds;
            }

            double GetNanoseconds()
            {
                return m_Nanoseconds;
            }

            double GetMilliseconds()
            {
                return m_Nanoseconds / 1000000;
            }

            double GetSeconds()
            {
                return m_Nanoseconds / 1000000000;
            }

        private:
            double m_Nanoseconds = 0;
    };
}