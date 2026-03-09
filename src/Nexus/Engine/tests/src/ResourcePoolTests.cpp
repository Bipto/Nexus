#include <gtest/gtest.h>

#include "Nexus-Core/Graphics/ResourcePool.hpp"

TEST(ResourcePool, Successful)
{
	struct TestResource
	{
	};

	struct TestTag
	{
	};

	using TestHandle = Nexus::Graphics::HandleT<TestTag>;

	Nexus::Graphics::ResourcePool<TestResource, TestHandle> pool;

	auto handle	 = pool.Create(TestResource {});
	auto handle2 = pool.Create(TestResource {});

	pool.Destroy(handle);
	auto handle3 = pool.Create(TestResource {});

	EXPECT_EQ(handle3.GetIndex(), 0);
	EXPECT_EQ(handle3.GetGeneration(), 1);
}