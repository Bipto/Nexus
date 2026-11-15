#pragma once

#include <Maths/Numeric.hpp>
#include <Maths/Vector2.hpp>

namespace Nexus::Maths
{
    template <Numeric T>
    Vector2<T>::Vector2(T x, T y) : X{x}, Y{y}
    {
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Add(Vector2<T> a, Vector2<T> b)
    {
        return Vector2<T>(a.X + b.X, a.Y + b.Y);
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Subtract(Vector2<T> a, Vector2<T> b)
    {
        return Vector2<T>(a.X - b.X, a.Y - b.Y);
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Multiply(Vector2<T> a, Vector2<T> b)
    {
        return Vector2<T>(a.X * b.X, a.Y * b.Y);
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Divide(Vector2<T> a, Vector2<T> b)
    {
        return Vector2<T>(a.X / b.X, a.Y / b.Y);
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Modulus(Vector2<T> a, Vector2<T> b)
    {
        return Vector2<T>(a.X % b.X, a.Y % b.Y);
    }

    template <Numeric T>
    Vector2<T> Vector2<T>::Negate(Vector2<T> v)
    {
        return Vector2<T>(-v.X, -v.Y);
    }
} // namespace Nexus::Maths