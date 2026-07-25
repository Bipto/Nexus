#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageCopyTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageCopyTests, CopyBufferToBufferStoresIndices)
{
    Nexus::Graphics::BufferCopyDescription desc{};
    desc.Copies.resize(3);

    Storage.CopyBufferToBuffer(desc);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 2u);

    auto *header = FirstCommand(Storage);

    ASSERT_EQ(header->Type, Nexus::Graphics::CommandType::CopyBufferToBuffer);

    auto *cmd = GetCommand<Nexus::Graphics::BufferCopyCommandStorage>(header);

    EXPECT_EQ(cmd->SourceIndex, 0u);
    EXPECT_EQ(cmd->DestinationIndex, 1u);
}

TEST_F(CommandListStorageCopyTests, CopyBufferToBufferCopiesPayload)
{
    Nexus::Graphics::BufferCopyDescription desc{};

    Nexus::Graphics::BufferCopy copy1{};
    copy1.ReadOffset = 16;
    copy1.WriteOffset = 64;
    copy1.Size = 128;

    Nexus::Graphics::BufferCopy copy2{};
    copy2.ReadOffset = 256;
    copy2.WriteOffset = 512;
    copy2.Size = 1024;

    desc.Copies = {copy1, copy2};

    Storage.CopyBufferToBuffer(desc);

    auto *header = FirstCommand(Storage);

    auto *payload = GetPayloadAs<Nexus::Graphics::BufferCopyCommandStorage, Nexus::Graphics::BufferCopy>(header);

    EXPECT_EQ(payload[0].ReadOffset, copy1.ReadOffset);

    EXPECT_EQ(payload[0].WriteOffset, copy1.WriteOffset);

    EXPECT_EQ(payload[0].Size, copy1.Size);

    EXPECT_EQ(payload[1].ReadOffset, copy2.ReadOffset);

    EXPECT_EQ(payload[1].WriteOffset, copy2.WriteOffset);

    EXPECT_EQ(payload[1].Size, copy2.Size);
}

TEST_F(CommandListStorageCopyTests, CopyBufferToBufferSupportsEmptyCopyList)
{
    Nexus::Graphics::BufferCopyDescription desc{};

    Storage.CopyBufferToBuffer(desc);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::CopyBufferToBuffer);

    auto *cmd = GetCommand<Nexus::Graphics::BufferCopyCommandStorage>(header);

    EXPECT_EQ(cmd->SourceIndex, 0u);
    EXPECT_EQ(cmd->DestinationIndex, 1u);
}

TEST_F(CommandListStorageCopyTests, CopyBufferToTextureStoresDescription)
{
    Nexus::Graphics::BufferTextureCopyDescription desc{};

    desc.BufferOffset = 100;
    desc.BufferRowLength = 256;
    desc.BufferImageHeight = 128;

    desc.TextureOffset = {1, 2, 3};
    desc.TextureExtent = {512, 256};

    desc.MipLevel = 4;

    Storage.CopyBufferToTexture(desc);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 1u);
    ASSERT_EQ(Storage.Textures.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::BufferTextureCopyCommandStorage>(Storage);

    EXPECT_EQ(cmd->BufferIndex, 0u);
    EXPECT_EQ(cmd->TextureIndex, 0u);

    EXPECT_EQ(cmd->BufferOffset, 100u);
    EXPECT_EQ(cmd->BufferRowLength, 256u);
    EXPECT_EQ(cmd->BufferImageHeight, 128u);

    EXPECT_EQ(cmd->TextureOffset.X, desc.TextureOffset.X);
    EXPECT_EQ(cmd->TextureOffset.Y, desc.TextureOffset.Y);
    EXPECT_EQ(cmd->TextureOffset.Z, desc.TextureOffset.Z);

    EXPECT_EQ(cmd->TextureExtent.Width, desc.TextureExtent.Width);
    EXPECT_EQ(cmd->TextureExtent.Height, desc.TextureExtent.Height);

    EXPECT_EQ(cmd->MipLevel, 4u);
}

TEST_F(CommandListStorageCopyTests, CopyTextureToBufferStoresDescription)
{
    Nexus::Graphics::BufferTextureCopyDescription desc{};

    desc.BufferOffset = 50;
    desc.BufferRowLength = 128;
    desc.BufferImageHeight = 32;
    desc.MipLevel = 7;

    Storage.CopyTextureToBuffer(desc);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::CopyTextureToBuffer);

    auto *cmd = GetCommand<Nexus::Graphics::BufferTextureCopyCommandStorage>(header);

    EXPECT_EQ(cmd->BufferIndex, 0u);
    EXPECT_EQ(cmd->TextureIndex, 0u);

    EXPECT_EQ(cmd->BufferOffset, 50u);
    EXPECT_EQ(cmd->MipLevel, 7u);
}

TEST_F(CommandListStorageCopyTests, CopyTextureToTextureStoresDescription)
{
    Nexus::Graphics::TextureCopyDescription desc{};

    desc.SourceOffset = {1, 2, 3};
    desc.DestinationOffset = {4, 5, 6};
    desc.Extent = {512, 512};

    desc.SourceMipLevel = 2;
    desc.DestinationMipLevel = 5;

    Storage.CopyTextureToTexture(desc);

    ASSERT_EQ(Storage.Textures.size(), 2u);

    auto *cmd = GetCommand<Nexus::Graphics::TextureCopyCommandStorage>(Storage);

    EXPECT_EQ(cmd->SourceTextureIndex, 0u);
    EXPECT_EQ(cmd->DestinationTextureIndex, 1u);

    EXPECT_EQ(cmd->SourceOffset.X, desc.SourceOffset.X);
    EXPECT_EQ(cmd->SourceOffset.Y, desc.SourceOffset.Y);
    EXPECT_EQ(cmd->SourceOffset.Z, desc.SourceOffset.Z);

    EXPECT_EQ(cmd->DestinationOffset.X, desc.DestinationOffset.X);
    EXPECT_EQ(cmd->DestinationOffset.Y, desc.DestinationOffset.Y);
    EXPECT_EQ(cmd->DestinationOffset.Z, desc.DestinationOffset.Z);

    EXPECT_EQ(cmd->Extent.Width, desc.Extent.Width);
    EXPECT_EQ(cmd->Extent.Height, desc.Extent.Height);

    EXPECT_EQ(cmd->SourceMipLevel, 2u);
    EXPECT_EQ(cmd->DestinationMipLevel, 5u);
}

TEST_F(CommandListStorageCopyTests, ResolveFramebufferStoresSeparateIndices)
{
    Nexus::Graphics::ResolveTextureDescription desc{};

    Storage.ResolveFramebuffer(desc);

    auto *cmd = GetCommand<Nexus::Graphics::ResolveTextureCommandStorage>(Storage);

    EXPECT_EQ(cmd->SourceTextureIndex, 0u);

    EXPECT_EQ(cmd->DestinationTextureIndex, 1u);
}

TEST_F(CommandListStorageCopyTests, StartTimingQueryStoresIndex)
{
    Storage.StartTimingQuery({});

    ASSERT_EQ(Storage.TimingQueries.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::TimingQueryCommandStorage>(Storage);

    EXPECT_EQ(cmd->QueryIndex, 0u);
}

TEST_F(CommandListStorageCopyTests, TimingQueryIndicesIncrease)
{
    Storage.StartTimingQuery({});
    Storage.StopTimingQuery({});

    auto *first = FirstCommand(Storage);
    auto *second = NextCommand(first);

    EXPECT_EQ(GetCommand<Nexus::Graphics::TimingQueryCommandStorage>(first)->QueryIndex, 0u);

    EXPECT_EQ(GetCommand<Nexus::Graphics::TimingQueryCommandStorage>(second)->QueryIndex, 1u);
}