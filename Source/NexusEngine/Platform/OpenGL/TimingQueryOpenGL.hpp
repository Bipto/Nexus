#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/TimingQuery.hpp"

namespace Nexus::Graphics
{
    class TimingQueryOpenGL : public TimingQuery
    {
    public:
        TimingQueryOpenGL();
        virtual void Resolve() override;
        virtual float GetElapsedMilliseconds() override;

    private:
        uint64_t m_Start = 0;
        uint64_t m_End = 0;
        float m_ElapsedTime = 0;
        friend class CommandListOpenGL;
    };

}

#endif