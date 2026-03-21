#include <gtest/gtest.h>

#include "Nexus-Core/Resources/MemoryResourceLoader.hpp"

#include <atomic>
#include <thread>

class MemoryResourceLoaderTest : public ::testing::Test
{
  protected:
	Nexus::MemoryResourceLoader loader;
};

// Helper to create byte vectors from initializer lists
static std::vector<std::byte> Bytes(std::initializer_list<uint8_t> values)
{
	std::vector<std::byte> out;
	out.reserve(values.size());
	for (auto v : values) { out.push_back(static_cast<std::byte>(v)); }
	return out;
}

TEST_F(MemoryResourceLoaderTest, LoadFailsWhenFileNotMounted)
{
	auto result = loader.Load("missing.bin");
	ASSERT_FALSE(result.has_value());
	EXPECT_FALSE(result);
	EXPECT_EQ(result.error(), "Failed to find data mounted to given path: missing.bin");
}

TEST_F(MemoryResourceLoaderTest, MountFileSucceedsForNewFile)
{
	auto data		 = Bytes({1, 2, 3});
	auto mountResult = loader.MountFile("file.bin", false, data);

	ASSERT_TRUE(mountResult.has_value());

	auto loadResult = loader.Load("file.bin");
	ASSERT_TRUE(loadResult.has_value());
	EXPECT_EQ(loadResult.value(), data);
}

TEST_F(MemoryResourceLoaderTest, MountFileFailsIfExistsAndNoOverwrite)
{
	auto data1 = Bytes({1, 2});
	auto data2 = Bytes({9, 9});

	ASSERT_TRUE(loader.MountFile("file.bin", false, data1).has_value());

	auto mountResult = loader.MountFile("file.bin", false, data2);

	ASSERT_FALSE(mountResult.has_value());
	EXPECT_EQ(mountResult.error(), "Attempting to mount file to path, however a file already exists and overwrite is disabled: file.bin");
}

TEST_F(MemoryResourceLoaderTest, MountFileOverwritesWhenAllowed)
{
	auto data1 = Bytes({1, 2});
	auto data2 = Bytes({9, 9});

	ASSERT_TRUE(loader.MountFile("file.bin", false, data1).has_value());

	auto mountResult = loader.MountFile("file.bin", true, data2);
	ASSERT_TRUE(mountResult.has_value());

	auto loadResult = loader.Load("file.bin");
	ASSERT_TRUE(loadResult.has_value());
	EXPECT_EQ(loadResult.value(), data2);
}

TEST_F(MemoryResourceLoaderTest, LoadReturnsCorrectData)
{
	auto data = Bytes({10, 20, 30, 40});
	ASSERT_TRUE(loader.MountFile("assets/texture.png", false, data).has_value());

	auto result = loader.Load("assets/texture.png");
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result.value(), data);
}

TEST_F(MemoryResourceLoaderTest, ConcurrentLoadsAreThreadSafe)
{
	auto data = Bytes({1, 2, 3, 4, 5});
	ASSERT_TRUE(loader.MountFile("shared.bin", false, data).has_value());

	constexpr int THREAD_COUNT = 32;
	constexpr int ITERATIONS   = 5000;

	std::vector<std::thread> threads;
	std::atomic<bool>		 failed = false;

	for (int t = 0; t < THREAD_COUNT; ++t)
	{
		threads.emplace_back(
			[&]()
			{
				for (int i = 0; i < ITERATIONS; ++i)
				{
					auto result = loader.Load("shared.bin");
					if (!result.has_value() || result.value() != data)
					{
						failed = true;
						return;
					}
				}
			});
	}

	for (auto &th : threads) th.join();

	EXPECT_FALSE(failed);
}

TEST_F(MemoryResourceLoaderTest, ConcurrentMountAndLoadIsThreadSafe)
{
	constexpr int THREAD_COUNT = 32;
	constexpr int FILE_COUNT   = 200;

	std::vector<std::thread> threads;
	std::atomic<bool>		 failed = false;

	// Pre-generate data
	std::vector<std::vector<std::byte>> testData;
	for (int i = 0; i < FILE_COUNT; ++i) testData.push_back(Bytes({uint8_t(i), uint8_t(i + 1), uint8_t(i + 2)}));

	// Launch threads
	for (int t = 0; t < THREAD_COUNT; ++t)
	{
		threads.emplace_back(
			[&, t]()
			{
				for (int i = 0; i < FILE_COUNT; ++i)
				{
					std::string path = "file_" + std::to_string(i) + ".bin";

					// Randomly mount or load
					if ((t + i) % 2 == 0)
					{
						auto r = loader.MountFile(path, true, testData[i]);
						if (!r.has_value())
						{
							failed = true;
							return;
						}
					}
					else
					{
						auto r = loader.Load(path);
						if (r.has_value() && r.value() != testData[i])
						{
							failed = true;
							return;
						}
					}
				}
			});
	}

	for (auto &th : threads) th.join();

	EXPECT_FALSE(failed);
}
