#include <gtest/gtest.h>

#include "Nexus-Core/Point.hpp"

TEST(Point3D, To)
{
    Nexus::Point3D<int> value(5, 7, 2);
    Nexus::Point3D<float> result = value.To<float>();

    EXPECT_EQ(result.X, 5.0f);
    EXPECT_EQ(result.Y, 7.0f);
    EXPECT_EQ(result.Z, 2.0f);
}

TEST(Point3D, Deconstruct)
{
    Nexus::Point3D<int> value(5, 7, 2);

    int x, y, z;
    value.Deconstruct(&x, &y, &z);

    EXPECT_EQ(value.X, x);
    EXPECT_EQ(value.Y, y);
    EXPECT_EQ(value.Z, z);
}