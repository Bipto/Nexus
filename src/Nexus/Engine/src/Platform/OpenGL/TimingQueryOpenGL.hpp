#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include <cstdint>

#include "RHI/TimingQuery.hpp"

namespace Nexus::Graphics
{
    class TimingQueryOpenGL : public ITimingQuery
    {
      public:
        TimingQueryOpenGL();
        virtual void Resolve() override;
        virtual float GetElapsedMilliseconds() override;
        uint64_t m_Start = 0;
        uint64_t m_End = 0;
        float m_ElapsedTime = 0;
    };

} // namespace Nexus::Graphics

#endif