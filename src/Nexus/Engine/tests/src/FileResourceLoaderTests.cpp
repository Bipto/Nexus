#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "Nexus-Core/Resources/FileResourceLoader.hpp"

namespace fs = std::filesystem;

class FileResourceLoaderTest : public ::testing::Test
{
  protected:
	fs::path tempDir;

	void SetUp() override
	{
		tempDir = fs::temp_directory_path() / "file_loader_test";
		fs::create_directories(tempDir);
	}

	void TearDown() override
	{
		fs::remove_all(tempDir);
	}

	void WriteFile(const fs::path &path, std::string_view content)
	{
		std::ofstream out(path, std::ios::binary);
		out << content;
	}
};

TEST_F(FileResourceLoaderTest, LoadsExistingFile)
{
	fs::path filePath = tempDir / "hello.txt";
	WriteFile(filePath, "Hello world");

	Nexus::FileResourceLoader loader(tempDir.string());
	auto					  result = loader.Load("hello.txt");

	ASSERT_TRUE(result.has_value());
	std::string loaded(reinterpret_cast<const char *>(result.value().data()), result.value().size());
	EXPECT_EQ(loaded, "Hello world");
}

TEST_F(FileResourceLoaderTest, ReturnsErrorForMissingFile)
{
	Nexus::FileResourceLoader loader(tempDir.string());
	auto					  result = loader.Load("does_not_exist.txt");

	ASSERT_FALSE(result.has_value());
	EXPECT_FALSE(result.error().empty());
}

TEST_F(FileResourceLoaderTest, RespectsBaseDirectory)
{
	// Create a file outside the base directory
	fs::path outside = tempDir.parent_path() / "outside.txt";
	WriteFile(outside, "Should not load");

	Nexus::FileResourceLoader loader(tempDir.string());
	auto					  result = loader.Load("../outside.txt");

	ASSERT_FALSE(result.has_value());
	EXPECT_FALSE(result.error().empty());
}

TEST_F(FileResourceLoaderTest, LoadsEmptyFile)
{
	fs::path filePath = tempDir / "empty.bin";
	WriteFile(filePath, "");

	Nexus::FileResourceLoader loader(tempDir.string());
	auto					  result = loader.Load("empty.bin");

	ASSERT_TRUE(result.has_value());
	EXPECT_TRUE(result->empty());
}

TEST_F(FileResourceLoaderTest, LoadsBinaryDataCorrectly)
{
	fs::path filePath = tempDir / "binary.bin";

	std::vector<unsigned char> bytes = {0x00, 0xFF, 0x10, 0x42};
	std::ofstream			   out(filePath, std::ios::binary);
	out.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
	out.close();

	Nexus::FileResourceLoader loader(tempDir.string());

	auto result = loader.Load("binary.bin");

	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result->size(), bytes.size());

	for (size_t i = 0; i < bytes.size(); ++i) { EXPECT_EQ(static_cast<unsigned char>((*result)[i]), bytes[i]); }
}
