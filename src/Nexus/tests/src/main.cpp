#include <gtest/gtest.h>

#include "Nexus-Core/Graphics/Circle.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

#include "Nexus-Core/Events/EventHandler.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

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

TEST(Point3D, To)
{
	Nexus::Point3D<int>	  value(5, 7, 2);
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

TEST(ScopedEvent, Behaviour)
{
	Nexus::EventHandler<> eventHandler;

	EXPECT_EQ(eventHandler.GetDelegateCount(), 0);

	{
		Nexus::ScopedEvent<> event(&eventHandler, []() {});
		EXPECT_EQ(eventHandler.GetDelegateCount(), 1);
	}

	EXPECT_EQ(eventHandler.GetDelegateCount(), 0);
}

#if defined(NX_PLATFORM_OPENGL)
TEST(CreateGraphicsDeviceOpenGL, Valid)
{
	Nexus::Graphics::GraphicsDeviceSpecification spec;
	spec.API								= Nexus::Graphics::GraphicsAPI::OpenGL;
	Nexus::Graphics::GraphicsDevice *device = Nexus::Graphics::GraphicsDevice::CreateGraphicsDevice(spec);
	EXPECT_TRUE(device->Validate());
}
#endif

#if defined(NX_PLATFORM_D3D12)
TEST(CreateGraphicsDeviceD3D12, Valid)
{
	Nexus::Graphics::GraphicsDeviceSpecification spec;
	spec.API								= Nexus::Graphics::GraphicsAPI::D3D12;
	Nexus::Graphics::GraphicsDevice *device = Nexus::Graphics::GraphicsDevice::CreateGraphicsDevice(spec);
	EXPECT_TRUE(device->Validate());
}
#endif

#if defined(NX_PLATFORM_VULKAN)
TEST(CreateGraphicsDeviceVulkan, Valid)
{
	Nexus::Graphics::GraphicsDeviceSpecification spec;
	spec.API								= Nexus::Graphics::GraphicsAPI::Vulkan;
	Nexus::Graphics::GraphicsDevice *device = Nexus::Graphics::GraphicsDevice::CreateGraphicsDevice(spec);
	EXPECT_TRUE(device->Validate());
}
#endif