#pragma once

//-----------------------------------------------------------------------------
// POINT
//-----------------------------------------------------------------------------

namespace Nexus
{
    /// @brief  A struct containing a pair of values
    /// @tparam T A type to contain within the point
    template <typename T>
    struct Point
    {
    public:
        /// @brief A default constructor creating an empty object
        Point() = default;

        /// @brief A constructor that takes in a single value and assigns it to both values
        /// @param x The value to assign to the the pair of values
        Point(T x)
            : X(x), Y(x) {}

        /// @brief A constructor that takes in a pair of values
        /// @param x The first value to assign
        /// @param y The second value to assign
        Point(T x, T y)
            : X(x), Y(y) {}

        template <typename Other>
        const Point<Other> To() const
        {
            return Point<Other>((Other)X, (Other)Y);
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
}
