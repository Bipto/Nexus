#include <gtest/gtest.h>

#include "Core/ThreadPool.hpp"

TEST(ThreadPool, ExecutesJobs)
{
	Nexus::ThreadPool pool("TestPool", 4);

	std::atomic<int> counter = 0;

	for (int i = 0; i < 10; ++i)
	{
		pool.Submit([&] { counter++; });
	}

	pool.WaitUntilIdle();
	EXPECT_EQ(counter.load(), 10);
}

TEST(ThreadPool, ReturnsFutureResults)
{
	Nexus::ThreadPool pool("TestPool", 4);

	auto f1 = pool.Submit([] { return 10; });
	auto f2 = pool.Submit([](int x) { return x * 2; }, 21);

	EXPECT_EQ(f1.get(), 10);
	EXPECT_EQ(f2.get(), 42);
}

TEST(ThreadPool, WaitUntilIdleWorks)
{
	Nexus::ThreadPool pool("TestPool", 4);

	std::atomic<int> counter = 0;

	for (int i = 0; i < 20; ++i)
	{
		pool.Submit(
			[&]
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				counter++;
			});
	}

	pool.WaitUntilIdle();
	EXPECT_EQ(counter.load(), 20);
}

TEST(ThreadPool, StopPreventsSubmit)
{
	Nexus::ThreadPool pool("TestPool", 2);

	pool.Stop();

	EXPECT_THROW(pool.Submit([] {}), std::runtime_error);
}

TEST(ThreadPool, JoinStopsWorkers)
{
	Nexus::ThreadPool pool("TestPool", 2);

	std::atomic<int> counter = 0;

	pool.Submit(
		[&]
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			counter++;
		});

	pool.Stop();
	pool.Join();

	EXPECT_EQ(counter.load(), 1);
}

TEST(ThreadPool, PauseAndResume)
{
	Nexus::ThreadPool pool("TestPool", 2);

	std::atomic<int> counter = 0;

	pool.Pause();

	for (int i = 0; i < 5; ++i)
	{
		pool.Submit([&] { counter++; });
	}

	// Give workers time to *not* run
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	EXPECT_EQ(counter.load(), 0);

	pool.Resume();
	pool.WaitUntilIdle();

	EXPECT_EQ(counter.load(), 5);
}

TEST(ThreadPool, BoundedQueueBlocks)
{
	Nexus::ThreadPool pool("TestPool", 1, /*maxQueueSize=*/1);

	std::atomic<int> counter = 0;

	// First job occupies the worker
	pool.Submit(
		[&]
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			counter++;
		});

	// Second job fills the queue
	pool.Submit([&] { counter++; });

	// Third job should block until one of the above finishes
	auto start = std::chrono::steady_clock::now();
	pool.Submit([&] { counter++; });
	auto end = std::chrono::steady_clock::now();

	EXPECT_GE((end - start), std::chrono::milliseconds(40));
}

TEST(ThreadPool, StressTest)
{
	Nexus::ThreadPool pool("Stress", 8);

	std::atomic<int> counter = 0;

	std::vector<std::future<void>> futures;
	futures.reserve(1000);

	for (int i = 0; i < 1000; ++i)
	{
		futures.push_back(pool.Submit([&] { counter++; }));
	}

	for (auto &f : futures) { f.get(); }

	EXPECT_EQ(counter.load(), 1000);
}