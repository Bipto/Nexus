#pragma once

#include "Triangle.hpp"

#include <vector>

namespace Nexus::Graphics
{
    template <typename T>
    class EarClipper
    {
    public:
        explicit EarClipper() = default;
        std::vector<Triangle<T>> Build(const std::vector<Point<T>> &points)
        {
            std::vector<Triangle<T>> triangles;

            return triangles;
        }
    };
}