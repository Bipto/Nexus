#include <gtest/gtest.h>

#include "Nexus-Core/Point.hpp"

TEST(Point2D, To)
{
	Nexus::Point2D<int>	  value(5, 7);
	Nexus::Point2D<float> result = value.To<float>();

	EXPECT_EQ(result.X, 5.0f);
	EXPECT_EQ(result.Y, 7.0f);
}

TEST(Point2D, Deconstruct)
{
	Nexus::Point2D<int> value(5, 7);

	int x, y;
	value.Deconstruct(&x, &y);

	EXPECT_EQ(value.X, x);
	EXPECT_EQ(value.Y, y);
}