#include <gtest/gtest.h>

#include <atomic>
#include <exception>
#include <latch>
#include <stop_token>
#include <string>
#include <thread>

#include "Core/NamedJThread.hpp"

namespace
{
	inline std::string GetCurrentThreadName()
	{
#if defined(__linux__)
		// Linux limit: 16 bytes including null terminator
		char buf[16] = {};
		pthread_getname_np(pthread_self(), buf, sizeof(buf));
		return std::string(buf);

#elif defined(__APPLE__)
		// macOS allows longer names; 64 bytes is safe
		char buf[64] = {};
		pthread_getname_np(pthread_self(), buf, sizeof(buf));
		return std::string(buf);

#elif defined(_WIN32)
		PWSTR wname = nullptr;
		GetThreadDescription(GetCurrentThread(), &wname);

		std::wstring ws = wname ? wname : L"";
		if (wname)
		{
			LocalFree(wname);
		}

		return std::string(ws.begin(), ws.end());

#else
		return {};
#endif }
	}
}

TEST(NamedJThread, ThreadName)
{
	std::latch	ready(1);
	std::string observedName = {};

	Nexus::NamedJThread namedThread("test_thread",
									nullptr,
									nullptr,
									nullptr,
									[&](std::stop_token)
									{
										observedName = GetCurrentThreadName();
										ready.count_down();
									});

	ready.wait();

	EXPECT_EQ(observedName, "test_thread");
}

TEST(NamedJThread, ThreadID)
{
	std::latch ready(1);

	Nexus::NamedJThread namedThread("test_thread", nullptr, nullptr, nullptr, [&](std::stop_token) {});
	namedThread.Join();

	EXPECT_NE(namedThread.GetID(), std::this_thread::get_id());
}

TEST(NamedJThread, Cancellation)
{
	std::atomic<bool> stopped = false;

	Nexus::NamedJThread t("cancellable",
						  nullptr,
						  nullptr,
						  nullptr,
						  [&](std::stop_token st)
						  {
							  while (!st.stop_requested()) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
							  stopped = true;
						  });

	t.RequestStop();
	t.Join();

	EXPECT_TRUE(stopped.load());
}

TEST(NamedJThread, WaitUntilStartedUnblocks)
{
	std::atomic<bool> ran = false;

	Nexus::NamedJThread t("test", nullptr, nullptr, nullptr, [&](std::stop_token) { ran = true; });

	t.WaitUntilStarted();
	t.WaitUntilStopped();
	EXPECT_TRUE(ran.load());
}

TEST(NamedJThread, WaitUntilStoppedUnblocks)
{
	Nexus::NamedJThread t("test", nullptr, nullptr, nullptr, [&](std::stop_token) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });

	t.WaitUntilStopped();
	EXPECT_FALSE(t.IsRunning());
}

TEST(NamedJThread, LifecycleCallbacksFire)
{
	std::atomic<bool> started = false;
	std::atomic<bool> stopped = false;

	Nexus::NamedJThread t(
		"test",
		[&]() { started = true; },
		[&]() { stopped = true; },
		nullptr,
		[&](std::stop_token) { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });

	t.Join();

	EXPECT_TRUE(started.load());
	EXPECT_TRUE(stopped.load());
}

TEST(NamedJThread, OnExceptionFires)
{
	std::atomic<bool> exceptionCaught = false;

	Nexus::NamedJThread t(
		"test",
		[]() {},
		[]() {},
		[&](std::exception_ptr exception) { exceptionCaught = true; },
		[&](std::stop_token) { throw std::runtime_error("boom"); });

	EXPECT_THROW(t.Join(), std::runtime_error);
	EXPECT_TRUE(exceptionCaught.load());
}

TEST(NamedJThread, CancellationStopsThread)
{
	std::atomic<bool> stopped = false;

	Nexus::NamedJThread t("test",
						  nullptr,
						  nullptr,
						  nullptr,
						  [&](std::stop_token st)
						  {
							  while (!st.stop_requested()) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
							  stopped = true;
						  });

	t.RequestStop();
	t.Join();

	EXPECT_TRUE(stopped.load());
}

TEST(NamedJThread, UptimeIsPositive)
{
	Nexus::NamedJThread t("test", nullptr, nullptr, nullptr, [&](std::stop_token) { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });

	t.Join();
	EXPECT_GT(t.Uptime(), std::chrono::milliseconds(0));
}

TEST(NamedJThread, RunningStateTransitions)
{
	std::atomic<bool> inside = false;

	Nexus::NamedJThread t("test",
						  nullptr,
						  nullptr,
						  nullptr,
						  [&](std::stop_token)
						  {
							  inside = true;
							  std::this_thread::sleep_for(std::chrono::milliseconds(5));
						  });

	t.WaitUntilStarted();
	EXPECT_TRUE(t.IsRunning());

	t.Join();
	EXPECT_FALSE(t.IsRunning());
}

TEST(NamedJThread, DescribeReturnsString)
{
	Nexus::NamedJThread t("test", nullptr, nullptr, nullptr, [&](std::stop_token) {});
	t.Join();

	auto desc = t.Describe();
	EXPECT_FALSE(desc.empty());
	EXPECT_NE(desc.find("test"), std::string::npos);
}

TEST(NamedJThread, ArgumentForwardingWorks)
{
	std::atomic<int> result = 0;

	Nexus::NamedJThread t("test", nullptr, nullptr, nullptr, [&](std::stop_token, int a, int b) { result = a + b; }, 3, 4);

	t.Join();
	EXPECT_EQ(result.load(), 7);
}