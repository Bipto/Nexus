#pragma once

#include <Maths/Numeric.hpp>

namespace Nexus::Maths
{
    template <Numeric T>
    struct Vector2
    {
        union {
            struct
            {
                T X, Y;
            };
            struct
            {
                T R, G;
            };
        };

        Vector2() = default;
        Vector2(T x, T y);

        static Vector2<T> Add(Vector2<T> a, Vector2<T> b);
        static Vector2<T> Subtract(Vector2<T> a, Vector2<T> b);
        static Vector2<T> Multiply(Vector2<T> a, Vector2<T> b);
        static Vector2<T> Divide(Vector2<T> a, Vector2<T> b);
        static Vector2<T> Modulus(Vector2<T> a, Vector2<T> b);
        static Vector2<T> Negate(Vector2<T> a);
    };
} // namespace Nexus::Maths

#include <Maths/Vector2.inl>