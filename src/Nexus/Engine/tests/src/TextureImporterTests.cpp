#include <gtest/gtest.h>

#include <fstream>

#include "Nexus-Core/Importers/TextureImporter.hpp"
#include "Nexus-Core/Resources/FileResourceLoader.hpp"

static std::vector<std::byte> LoadFile(const std::string &path)
{
	std::ifstream	  file(path, std::ios::binary);
	std::vector<char> tmp((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	return {reinterpret_cast<std::byte *>(tmp.data()), reinterpret_cast<std::byte *>(tmp.data() + tmp.size())};
}

TEST(TextureImporter, GetTextureInfoFromDisk_Success)
{
	Nexus::TextureImporter importer;

	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	auto					  result = importer.GetTextureInfoFromDisk(&loader, "resources/1x1_rgb.jpg");

	ASSERT_TRUE(result.has_value());
	auto info = result.value();

	EXPECT_EQ(info.Width, 1);
	EXPECT_EQ(info.Height, 1);
	EXPECT_EQ(info.Channels, 3);
	EXPECT_EQ(info.Type, Nexus::PixelType::Pixel8Bit);
}

TEST(TextureImporter, GetTextureInfoFromDisk_FileMissing)
{
	Nexus::TextureImporter importer;

	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	auto					  result = importer.GetTextureInfoFromDisk(&loader, "resources/does_not_exist.png");

	EXPECT_FALSE(result.has_value());
}

TEST(TextureImporter, GetTextureInfoFromMemory_InvalidBuffer)
{
	Nexus::TextureImporter importer;

	std::vector<std::byte> bad	  = {std::byte {0xFF}, std::byte {0x00}};
	auto				   result = importer.GetTextureInfoFromMemory(bad);

	EXPECT_FALSE(result.has_value());
}

TEST(TextureImporter, LoadImageFromDisk_Success)
{
	Nexus::TextureImporter importer;

	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	auto					  result = importer.LoadImageFromDisk(&loader, "resources/2x2_rgba.png", false, std::nullopt);

	ASSERT_TRUE(result.has_value());
	auto data = result.value();

	EXPECT_EQ(data.Info.Width, 2);
	EXPECT_EQ(data.Info.Height, 2);
	EXPECT_EQ(data.Info.Channels, 4);
	EXPECT_EQ(data.Pixels.size(), 2 * 2 * 4);
}

TEST(TextureImporter, LoadImageFromDisk_DesiredChannels)
{
	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	Nexus::TextureImporter	  importer;
	auto					  result = importer.LoadImageFromDisk(&loader, "resources/2x2_rgba.png", false, 3);

	ASSERT_TRUE(result.has_value());
	auto data = result.value();

	EXPECT_EQ(data.Info.Channels, 3);
	EXPECT_EQ(data.Pixels.size(), 2 * 2 * 3);
}

TEST(TextureImporter, LoadImageFromMemory_MatchesDisk)
{
	Nexus::TextureImporter importer;

	auto buffer = LoadFile("resources/1x1_rgb.jpg");

	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	auto					  disk = importer.LoadImageFromDisk(&loader, "resources/1x1_rgb.jpg", false, std::nullopt);
	auto					  mem  = importer.LoadImageFromMemory(buffer, false, std::nullopt);

	ASSERT_TRUE(disk.has_value());
	ASSERT_TRUE(mem.has_value());

	EXPECT_EQ(mem->Info.Width, disk->Info.Width);
	EXPECT_EQ(mem->Info.Height, disk->Info.Height);
	EXPECT_EQ(mem->Info.Channels, disk->Info.Channels);
	EXPECT_EQ(mem->Pixels, disk->Pixels);
}

TEST(TextureImporter, LoadImageFromDisk_FlipVertically)
{
	Nexus::TextureImporter importer;

	Nexus::FileResourceLoader loader(std::filesystem::current_path().string());
	auto					  normal  = importer.LoadImageFromDisk(&loader, "resources/flip_test.png", false, std::nullopt);
	auto					  flipped = importer.LoadImageFromDisk(&loader, "resources/flip_test.png", true, std::nullopt);

	ASSERT_TRUE(normal.has_value());
	ASSERT_TRUE(flipped.has_value());

	const auto &n = normal->Pixels;
	const auto &f = flipped->Pixels;

	size_t rowSize = normal->Info.Width * normal->Info.Channels;

	EXPECT_TRUE(std::equal(n.begin(), n.begin() + rowSize, f.end() - rowSize));
}