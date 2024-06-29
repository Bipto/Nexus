#pragma once

#include <inttypes.h>
#include <random>
#include <cmath>

namespace Nexus
{
    struct GUID
    {
        uint64_t Value = 0;

        GUID()
        {
            static std::random_device rd;
            static std::mt19937_64 e2(rd());
            static std::uniform_int_distribution<uint64_t> dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));

            Value = dist(e2);
        }

        GUID(uint64_t v)
        {
            Value = v;
        }

        operator uint64_t()
        {
            return Value;
        }
    };
}