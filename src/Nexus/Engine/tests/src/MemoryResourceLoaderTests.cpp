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
    for (auto v : values)
    {
        out.push_back(static_cast<std::byte>(v));
    }
    return out;
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_LoadFailsWhenFileNotMounted)
{
    auto result = loader.LoadBytes("missing.bin");
    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result);
    EXPECT_EQ(
        result.error(), "Failed to find data mounted to given path: missing.bin"
    );
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_MountFileSucceedsForNewFile)
{
    auto data = Bytes({1, 2, 3});
    auto mountResult = loader.MountBinaryFile("file.bin", false, data);

    ASSERT_TRUE(mountResult.has_value());

    auto loadResult = loader.LoadBytes("file.bin");
    ASSERT_TRUE(loadResult.has_value());
    EXPECT_EQ(loadResult.value(), data);
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_MountFileFailsIfExistsAndNoOverwrite)
{
    auto data1 = Bytes({1, 2});
    auto data2 = Bytes({9, 9});

    ASSERT_TRUE(loader.MountBinaryFile("file.bin", false, data1).has_value());

    auto mountResult = loader.MountBinaryFile("file.bin", false, data2);

    ASSERT_FALSE(mountResult.has_value());
    EXPECT_EQ(
        mountResult.error(), "Attempting to mount file to path, however a file "
                             "already exists and overwrite is disabled: file.bin"
    );
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_MountFileOverwritesWhenAllowed)
{
    auto data1 = Bytes({1, 2});
    auto data2 = Bytes({9, 9});

    ASSERT_TRUE(loader.MountBinaryFile("file.bin", false, data1).has_value());

    auto mountResult = loader.MountBinaryFile("file.bin", true, data2);
    ASSERT_TRUE(mountResult.has_value());

    auto loadResult = loader.LoadBytes("file.bin");
    ASSERT_TRUE(loadResult.has_value());
    EXPECT_EQ(loadResult.value(), data2);
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_LoadReturnsCorrectData)
{
    auto data = Bytes({10, 20, 30, 40});
    ASSERT_TRUE(
        loader.MountBinaryFile("assets/texture.png", false, data).has_value()
    );

    auto result = loader.LoadBytes("assets/texture.png");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), data);
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_ConcurrentLoadsAreThreadSafe)
{
    auto data = Bytes({1, 2, 3, 4, 5});
    ASSERT_TRUE(loader.MountBinaryFile("shared.bin", false, data).has_value());

    constexpr int THREAD_COUNT = 32;
    constexpr int ITERATIONS = 5000;

    std::vector<std::thread> threads;
    std::atomic<bool> failed = false;

    for (int t = 0; t < THREAD_COUNT; ++t)
    {
        threads.emplace_back([&]() {
            for (int i = 0; i < ITERATIONS; ++i)
            {
                auto result = loader.LoadBytes("shared.bin");
                if (!result.has_value() || result.value() != data)
                {
                    failed = true;
                    return;
                }
            }
        });
    }

    for (auto &th : threads)
        th.join();

    EXPECT_FALSE(failed);
}

TEST_F(MemoryResourceLoaderTest, LoadBytes_ConcurrentMountAndLoadIsThreadSafe)
{
    constexpr int THREAD_COUNT = 32;
    constexpr int FILE_COUNT = 200;

    std::vector<std::thread> threads;
    std::atomic<bool> failed = false;

    // Pre-generate data
    std::vector<std::vector<std::byte>> testData;
    for (int i = 0; i < FILE_COUNT; ++i)
        testData.push_back(Bytes({uint8_t(i), uint8_t(i + 1), uint8_t(i + 2)}));

    // Launch threads
    for (int t = 0; t < THREAD_COUNT; ++t)
    {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < FILE_COUNT; ++i)
            {
                std::string path = "file_" + std::to_string(i) + ".bin";

                // Randomly mount or load
                if ((t + i) % 2 == 0)
                {
                    auto r = loader.MountBinaryFile(path, true, testData[i]);
                    if (!r.has_value())
                    {
                        failed = true;
                        return;
                    }
                }
                else
                {
                    auto r = loader.LoadBytes(path);
                    if (r.has_value() && r.value() != testData[i])
                    {
                        failed = true;
                        return;
                    }
                }
            }
        });
    }

    for (auto &th : threads)
        th.join();

    EXPECT_FALSE(failed);
}

TEST_F(MemoryResourceLoaderTest, LoadString_ReturnsCorrectString)
{
    std::string text = "Hello memory loader";
    std::vector<std::byte> data(text.size());
    memcpy(data.data(), text.data(), text.size());

    ASSERT_TRUE(loader.MountBinaryFile("greeting.txt", false, data).has_value());

    auto result = loader.LoadString("greeting.txt");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), text);
}

TEST_F(MemoryResourceLoaderTest, LoadString_FailsWhenFileNotMounted)
{
    auto result = loader.LoadString("missing.txt");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(
        result.error(), "Failed to find data mounted to given path: missing.txt"
    );
}

TEST_F(MemoryResourceLoaderTest, LoadString_LoadsEmptyString)
{
    std::vector<std::byte> emptyData;

    ASSERT_TRUE(loader.MountBinaryFile("empty.txt", false, emptyData).has_value());

    auto result = loader.LoadString("empty.txt");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->empty());
}

TEST_F(MemoryResourceLoaderTest, LoadString_HandlesBinaryData)
{
    auto data = Bytes({0x00, 0xFF, 0x10, 0x42});
    ASSERT_TRUE(loader.MountBinaryFile("binary.bin", false, data).has_value());

    auto result = loader.LoadString("binary.bin");
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->size(), data.size());
    for (size_t i = 0; i < data.size(); ++i)
        EXPECT_EQ(
            static_cast<unsigned char>((*result)[i]),
            static_cast<unsigned char>(data[i])
        );
}

TEST_F(MemoryResourceLoaderTest, DoesFileExist_ReturnsTrueForMountedFile)
{
    auto data = Bytes({1, 2, 3});
    ASSERT_TRUE(loader.MountBinaryFile("exists.bin", false, data).has_value());

    EXPECT_TRUE(loader.DoesFileExist("exists.bin"));
}

TEST_F(MemoryResourceLoaderTest, DoesFileExist_ReturnsFalseForMissingFile)
{
    EXPECT_FALSE(loader.DoesFileExist("not_here.bin"));
}

TEST_F(MemoryResourceLoaderTest, DoesFileExist_WorksWithNestedPaths)
{
    auto data = Bytes({9, 9, 9});
    ASSERT_TRUE(
        loader.MountBinaryFile("assets/textures/wood.png", false, data).has_value()
    );

    EXPECT_TRUE(loader.DoesFileExist("assets/textures/wood.png"));
    EXPECT_FALSE(loader.DoesFileExist("assets/textures/stone.png"));
}
