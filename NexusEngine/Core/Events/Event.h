#pragma once

namespace Nexus
{
    template <typename T>
    class Event
    {
        public:
            Event() = delete;
            Event(const T& payload)
            {
                m_Payload = payload;
            }

            const T& GetPayload()
            {
                return m_Payload;
            }
        private:
            T m_Payload;
    };
}