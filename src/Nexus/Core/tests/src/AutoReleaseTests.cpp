#include "Core/AutoRelease.hpp"
#include <gtest/gtest.h>

#include <memory>

namespace
{
	constexpr int INVALID = -1;

	// A simple deleter that counts calls
	struct TestDeleter
	{
		std::shared_ptr<int> count = std::make_shared<int>(0);

		void operator()(int h) const noexcept
		{
			++(*count);
		}
	};
}	 // namespace

TEST(AutoReleaseTest, DefaultConstructedIsInvalid)
{
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar;

	EXPECT_EQ(ar.Get(), INVALID);
	EXPECT_FALSE(ar.IsValid());
	EXPECT_FALSE(static_cast<bool>(ar));
}

TEST(AutoReleaseTest, ConstructWithHandleUsesDefaultDeleter)
{
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar(5);

	EXPECT_EQ(ar.Get(), 5);
	EXPECT_TRUE(ar.IsValid());
}

TEST(AutoReleaseTest, ConstructWithHandleAndDeleterStoresBoth)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);

	EXPECT_EQ(ar.Get(), 10);
	EXPECT_TRUE(ar.IsValid());
	EXPECT_EQ(*del.count, 0);
}

TEST(AutoReleaseTest, DestructorInvokesDeleter)
{
	TestDeleter del;

	{
		Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);
		EXPECT_EQ(*del.count, 0);
	}

	EXPECT_EQ(*del.count, 1);
}

TEST(AutoReleaseTest, ResetDeletesOldHandle)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);

	ar.Reset(20);

	EXPECT_EQ(ar.Get(), 20);
	EXPECT_EQ(*del.count, 1);
}

TEST(AutoReleaseTest, ResetToInvalidDeletesHandle)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);

	ar.Reset();	   // newHandle = INVALID

	EXPECT_EQ(ar.Get(), INVALID);
	EXPECT_EQ(*del.count, 1);
}

TEST(AutoReleaseTest, ReleaseReturnsHandleAndPreventsDeletion)
{
	TestDeleter del;

	{
		Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);

		int h = ar.Release();
		EXPECT_EQ(h, 10);
		EXPECT_EQ(ar.Get(), INVALID);
		EXPECT_EQ(*del.count, 0);
	}

	EXPECT_EQ(*del.count, 0);
}

TEST(AutoReleaseTest, MoveConstructorTransfersOwnership)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar1(10, del);

	Nexus::AutoRelease<int, INVALID, TestDeleter> ar2(std::move(ar1));

	EXPECT_EQ(ar2.Get(), 10);
	EXPECT_EQ(ar1.Get(), INVALID);
}

TEST(AutoReleaseTest, MoveAssignmentTransfersOwnershipAndDeletesOld)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar1(10, del);
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar2(20, del);

	ar2 = std::move(ar1);

	EXPECT_EQ(ar2.Get(), 10);
	EXPECT_EQ(ar1.Get(), INVALID);
	EXPECT_EQ(*del.count, 1);	 // old ar2 handle deleted
}

TEST(AutoReleaseTest, MoveAssignmentSelfNoOp)
{
	TestDeleter									  del;
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar(10, del);

	ar = std::move(ar);

	EXPECT_EQ(ar.Get(), 10);
	EXPECT_EQ(*del.count, 0);
}

TEST(AutoReleaseTest, SwapExchangesHandles)
{
	TestDeleter del1, del2;

	Nexus::AutoRelease<int, INVALID, TestDeleter> ar1(10, del1);
	Nexus::AutoRelease<int, INVALID, TestDeleter> ar2(20, del2);

	swap(ar1, ar2);

	EXPECT_EQ(ar1.Get(), 20);
	EXPECT_EQ(ar2.Get(), 10);
}

TEST(AutoReleaseTest, PointerOperatorsWorkForPointerHandle)
{
	using Ptr = int *;

	struct PtrDeleter
	{
		std::shared_ptr<int> count = std::make_shared<int>(0);
		void				 operator()(Ptr p) const noexcept
		{
			++(*count);
		}
	};

	int		   value = 42;
	PtrDeleter del;

	Nexus::AutoRelease<Ptr, nullptr, PtrDeleter> ar(&value, del);

	EXPECT_EQ(*ar, 42);
	EXPECT_EQ(ar.Get(), &value);
	EXPECT_EQ(*ar.operator->(), 42);
	ar.Reset();
	EXPECT_EQ(*del.count, 1);
}