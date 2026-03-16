#include <gtest/gtest.h>

#include "Core/ResourcePool.hpp"

#include "Core/ResourcePool.hpp"
#include <gtest/gtest.h>

TEST(ResourcePool, CreateAndGet)
{
	struct TestResource
	{
		int value = 0;
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	auto  handle = pool.Create(TestResource {42});
	auto *res	 = pool.Get(handle);

	ASSERT_NE(res, nullptr);
	EXPECT_EQ(res->value, 42);
	EXPECT_TRUE(res != nullptr);
}

TEST(ResourcePool, DestroyMakesSlotReusable)
{
	struct TestResource
	{
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	auto h1 = pool.Create(TestResource {});
	auto h2 = pool.Create(TestResource {});

	pool.Destroy(h1);

	auto h3 = pool.Create(TestResource {});

	EXPECT_EQ(h3.GetIndex(), h1.GetIndex());
	EXPECT_EQ(h3.GetGeneration(), h1.GetGeneration() + 1);
}

TEST(ResourcePool, GetReturnsNullForDestroyedHandle)
{
	struct TestResource
	{
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	auto handle = pool.Create(TestResource {});
	pool.Destroy(handle);

	EXPECT_EQ(pool.Get(handle), nullptr);
}

TEST(ResourcePool, GetReturnsNullForStaleHandle)
{
	struct TestResource
	{
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	auto h1 = pool.Create(TestResource {});
	pool.Destroy(h1);
	auto h2 = pool.Create(TestResource {});

	// h1 is now stale: same index, older generation
	EXPECT_NE(h1.GetGeneration(), h2.GetGeneration());
	EXPECT_EQ(pool.Get(h1), nullptr);
}

TEST(ResourcePool, EmplaceConstructsInPlace)
{
	struct TestResource
	{
		int x		   = 0;
		int y		   = 0;
		TestResource() = default;
		TestResource(int a, int b) : x(a), y(b)
		{
		}
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	auto  handle = pool.Emplace(10, 20);
	auto *res	 = pool.Get(handle);

	ASSERT_NE(res, nullptr);
	EXPECT_EQ(res->x, 10);
	EXPECT_EQ(res->y, 20);
}

TEST(ResourcePool, DestroyInvalidHandleDoesNotCrash)
{
	struct TestResource
	{
	};
	struct TestTag
	{
	};
	using TestHandle = Nexus::HandleT<TestTag>;

	Nexus::ResourcePool<TestResource, TestHandle> pool;

	TestHandle invalid {0, 0, nullptr};
	pool.Destroy(invalid);
}
