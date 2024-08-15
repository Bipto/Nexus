#pragma once

//-----------------------------------------------------------------------------
// POINT
//-----------------------------------------------------------------------------

namespace Nexus
{
/// @brief  A struct containing a pair of values
/// @tparam T A type to contain within the point
template <typename T> struct Point2D
{
  public:
    /// @brief A default constructor creating an empty object
    Point2D() = default;

    /// @brief A constructor that takes in a single value and assigns it to both values
    /// @param x The value to assign to the the pair of values
    Point2D(T x) : X(x), Y(x)
    {
    }

    /// @brief A constructor that takes in a pair of values
    /// @param x The first value to assign
    /// @param y The second value to assign
    Point2D(T x, T y) : X(x), Y(y)
    {
    }

    template <typename Other> const Point2D<Other> To() const
    {
        return Point2D<Other>((Other)X, (Other)Y);
    }

    void Deconstruct(T *x, T *y)
    {
        if (x)
        {
            *x = X;
        }

        if (y)
        {
            *y = Y;
        }
    }

    /// @brief The first value stored within the point
    T X = 0;

    /// @brief The second value stored within the point
    T Y = 0;
};

/// @brief  A struct containing three values
/// @tparam T A type to contain within the point
template <typename T> struct Point3D
{
  public:
    /// @brief A default constructor creating an empty object
    Point3D() = default;

    /// @brief A constructor that takes in a single value and assigns it to both values
    /// @param x The value to assign to the three values
    Point3D(T x) : X(x), Y(x), Z(x)
    {
    }

    /// @brief A constructor that takes in a pair of values
    /// @param x The first value to assign
    /// @param y The second value to assign
    Point3D(T x, T y, T z) : X(x), Y(y), T(z)
    {
    }

    template <typename Other> const Point3D<Other> To() const
    {
        return Point3D<Other>((Other)X, (Other)Y, (Other)Z);
    }

    void Deconstruct(T *x, T *y, T *z)
    {
        if (x)
        {
            *x = X;
        }

        if (y)
        {
            *y = Y;
        }

        if (z)
        {
            *z = Z;
        }
    }

    /// @brief The first value stored within the point
    T X = 0;

    /// @brief The second value stored within the point
    T Y = 0;

    /// @brief The third value stored within the point
    T Z = 0;
};
} // namespace Nexus
