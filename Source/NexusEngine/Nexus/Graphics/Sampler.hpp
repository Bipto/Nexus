#pragma once

#include "SamplerState.hpp"

namespace Nexus::Graphics
{
    class Sampler
    {
    public:
        virtual ~Sampler() {}
        virtual const SamplerSpecification &GetSamplerSpecification() = 0;
    };
}