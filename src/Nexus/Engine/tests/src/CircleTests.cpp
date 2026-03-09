#include <gtest/gtest.h>

#include "Nexus-Core/Graphics/Circle.hpp"

TEST(Circle, To)
{
	const Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25);
	Nexus::Graphics::Circle<float>	   result = c.To<float>();

	EXPECT_EQ(result.GetPosition().X, 50.0f);
	EXPECT_EQ(result.GetPosition().Y, 50.0f);
}

TEST(Circle, ContainsValid)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);
	bool						 result = c.Contains(Nexus::Point2D<int>(50, 50));

	EXPECT_EQ(result, true);
}

TEST(Circle, ContainsInvalid)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);
	bool						 result = c.Contains(Nexus::Point2D<int>(20, 50));

	EXPECT_EQ(result, false);
}

TEST(Circle, Deconstruct)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);

	int x, y, radius;
	c.Deconstruct(&x, &y, &radius);

	EXPECT_EQ(x, 50);
	EXPECT_EQ(y, 50);
	EXPECT_EQ(radius, 25);
}