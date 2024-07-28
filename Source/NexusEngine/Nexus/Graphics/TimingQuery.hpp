#pragma once

#include "Nexus/nxpch.hpp"

namespace Nexus::Graphics
{
    class TimingQuery
    {
    public:
        virtual ~TimingQuery() {}
        virtual void Resolve() = 0;
        virtual float GetElapsedMilliseconds() = 0;
    };
}