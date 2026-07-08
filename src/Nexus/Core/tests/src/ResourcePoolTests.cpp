#include <gtest/gtest.h>

#include <memory>

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

    auto handle = pool.Create(std::make_unique<TestResource>(42));
    auto *res = pool.Get(handle);

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

    auto h1 = pool.Create(std::make_unique<TestResource>());
    auto h2 = pool.Create(std::make_unique<TestResource>());

    pool.Destroy(h1);

    auto h3 = pool.Create(std::make_unique<TestResource>());

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

    auto handle = pool.Create(std::make_unique<TestResource>());
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

    auto h1 = pool.Create(std::make_unique<TestResource>());
    pool.Destroy(h1);
    auto h2 = pool.Create(std::make_unique<TestResource>());

    // h1 is now stale: same index, older generation
    EXPECT_NE(h1.GetGeneration(), h2.GetGeneration());
    EXPECT_EQ(pool.Get(h1), nullptr);
}

TEST(ResourcePool, EmplaceConstructsInPlace)
{
    struct TestResource
    {
        int x = 0;
        int y = 0;
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

    auto handle = pool.Emplace(10, 20);
    auto *res = pool.Get(handle);

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

    TestHandle invalid{0, 0, nullptr};
    pool.Destroy(invalid);
}

TEST(ResourcePoolHandles, UniqueHandleDestroysOnScopeExit)
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

    TestHandle raw{};

    {
        auto unique = pool.EmplaceUnique(123);
        ASSERT_TRUE(unique.IsValid());

        raw = unique.Raw();
        EXPECT_NE(pool.Get(raw), nullptr);
        EXPECT_EQ(pool.Get(raw)->value, 123);
    }

    // UniqueHandle should have destroyed the resource
    EXPECT_EQ(pool.Get(raw), nullptr);
}

TEST(ResourcePoolHandles, UniqueHandleMoveTransfersOwnership)
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

    auto unique1 = pool.EmplaceUnique(55);
    ASSERT_TRUE(unique1.IsValid());

    TestHandle raw = unique1.Raw();

    auto unique2 = std::move(unique1);

    EXPECT_FALSE(unique1.IsValid());
    EXPECT_TRUE(unique2.IsValid());
    EXPECT_EQ(pool.Get(raw)->value, 55);
}

TEST(ResourcePoolHandles, UniqueHandleResetExplicitlyDestroysResource)
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

    auto unique = pool.EmplaceUnique(77);
    TestHandle raw = unique.Raw();

    ASSERT_TRUE(unique.IsValid());
    unique.Reset();

    EXPECT_FALSE(unique.IsValid());
    EXPECT_EQ(pool.Get(raw), nullptr);
}

TEST(ResourcePoolHandles, SharedHandleCopiesShareOwnership)
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

    auto shared1 = pool.EmplaceShared(10);
    ASSERT_TRUE(shared1.IsValid());

    TestHandle raw = shared1.Raw();

    {
        auto shared2 = shared1;
        auto shared3 = shared2;

        EXPECT_TRUE(shared1.IsValid());
        EXPECT_TRUE(shared2.IsValid());
        EXPECT_TRUE(shared3.IsValid());

        EXPECT_EQ(pool.Get(raw)->value, 10);
    }

    // shared1 still alive → resource must still exist
    EXPECT_NE(pool.Get(raw), nullptr);

    // Now destroy last reference
    shared1 = {};
    EXPECT_EQ(pool.Get(raw), nullptr);
}

TEST(ResourcePoolHandles, SharedHandleLastCopyDestroysResource)
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

    auto shared = pool.EmplaceShared(999);
    TestHandle raw = shared.Raw();

    ASSERT_TRUE(shared.IsValid());
    EXPECT_NE(pool.Get(raw), nullptr);

    shared = {}; // drop last reference

    EXPECT_EQ(pool.Get(raw), nullptr);
}

TEST(ResourcePoolHandles, SharedAndUniqueAreIndependent)
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

    auto shared = pool.EmplaceShared(1);
    TestHandle sharedRaw = shared.Raw();

    {
        auto unique = pool.EmplaceUnique(2);
        ASSERT_TRUE(unique.IsValid());
        EXPECT_EQ(unique->value, 2);
    }

    // Unique destroyed, shared still alive
    EXPECT_NE(pool.Get(sharedRaw), nullptr);
    EXPECT_EQ(shared->value, 1);
}

TEST(ResourcePoolHandles, SharedHandleDoesNotAccessStaleResource)
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

    auto shared = pool.EmplaceShared(5);
    TestHandle raw = shared.Raw();

    // Destroy underlying resource manually
    pool.Destroy(raw);

    // SharedHandle should now be invalid
    EXPECT_FALSE(shared.IsValid());
    EXPECT_EQ(pool.Get(raw), nullptr);
}