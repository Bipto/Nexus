#pragma once

#include "Nexus/Point.hpp"

#include <string>
#include <inttypes.h>

namespace Nexus
{
    struct Monitor
    {
        Point<int32_t> Position = {0, 0};
        Point<int32_t> Size = {0, 0};
        Point<int32_t> WorkPosition = {0, 0};
        Point<int32_t> WorkSize{0, 0};
        float DPI = 0.0f;
        std::string Name;
    };
}