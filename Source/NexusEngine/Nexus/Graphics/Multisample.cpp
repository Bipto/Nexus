#include "Multisample.hpp"

#include <stdexcept>

uint32_t Nexus::Graphics::GetSampleCount(MultiSamples samples)
{
    switch (samples)
    {
    case MultiSamples::SampleCount1:
        return 1;
    case MultiSamples::SampleCount2:
        return 2;
    case MultiSamples::SampleCount4:
        return 4;
    case MultiSamples::SampleCount8:
        return 8;
    default:
        throw std::runtime_error("Invalid sample count entered");
        return 0;
    }
}