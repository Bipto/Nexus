#pragma once

//-----------------------------------------------------------------------------
// POINT
//-----------------------------------------------------------------------------

namespace Nexus
{
    template <typename T>
    struct Point
    {
    public:
        Point() = default;
        Point(T x)
            : X(x), Y(x) {}
        Point(T x, T y)
            : X(x), Y(y) {}
        T X = 0, Y = 0;
    };
}
