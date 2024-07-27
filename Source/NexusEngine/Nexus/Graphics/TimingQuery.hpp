#pragma once

#include "Nexus/nxpch.hpp"

namespace Nexus::Graphics
{
    class TimingQuery
    {
    public:
        virtual ~TimingQuery() {}
        virtual float GetElapsedMilliseconds() = 0;
    };
}