#pragma once

#include <inttypes.h>

namespace Nexus::Graphics
{
    enum class MultiSamples
    {
        SampleCount1,
        SampleCount2,
        SampleCount4,
        SampleCount8
    };

    uint32_t GetSampleCount(MultiSamples samples);
}