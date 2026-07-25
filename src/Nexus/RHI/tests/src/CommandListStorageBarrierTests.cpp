#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageBarrierTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageBarrierTests, EmptyBarrierGroupStoresZeroCounts)
{
    Nexus::Graphics::BarrierGroupDescription desc{};

    Storage.SubmitBarrierGroup(desc);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::BarrierGroup);

    auto *cmd = GetCommand<Nexus::Graphics::BarrierGroupCommandStorage>(header);

    EXPECT_EQ(cmd->MemoryBarrierCount, 0u);
    EXPECT_EQ(cmd->TextureBarrierCount, 0u);
    EXPECT_EQ(cmd->BufferBarrierCount, 0u);
}

TEST_F(CommandListStorageBarrierTests, BarrierCountsAreRecorded)
{
    Nexus::Graphics::BarrierGroupDescription desc{};

    desc.MemoryBarriers.resize(2);
    desc.TextureBarriers.resize(3);
    desc.BufferBarriers.resize(4);

    Storage.SubmitBarrierGroup(desc);

    auto *cmd = GetCommand<Nexus::Graphics::BarrierGroupCommandStorage>(Storage);

    EXPECT_EQ(cmd->MemoryBarrierCount, 2u);
    EXPECT_EQ(cmd->TextureBarrierCount, 3u);
    EXPECT_EQ(cmd->BufferBarrierCount, 4u);
}

TEST_F(CommandListStorageBarrierTests, TextureBarrierPayloadIsCopied)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::BarrierGroupDescription desc{};

    Nexus::Graphics::TextureBarrierDesc barrier{};

    barrier.Texture = {};
    barrier.Layout = Nexus::Graphics::TextureLayout::ShaderReadOnlyOptimal;
    barrier.BeforeAccess = {};
    barrier.AfterAccess = {};
    barrier.BeforeStage = {};
    barrier.AfterStage = {};

    desc.TextureBarriers.push_back(barrier);

    storage.SubmitBarrierGroup(desc);

    auto *header = FirstCommand(storage);

    auto *payload =
        GetPayloadAs<Nexus::Graphics::BarrierGroupCommandStorage, Nexus::Graphics::TextureBarrierCommandStorage>(
            header
        );

    EXPECT_EQ(payload[0].Layout, barrier.Layout);
}

TEST_F(CommandListStorageBarrierTests, BufferBarrierStoresRange)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::BarrierGroupDescription desc{};

    Nexus::Graphics::BufferBarrierDesc barrier{};

    barrier.Offset = 256;
    barrier.Size = 4096;

    desc.BufferBarriers.push_back(barrier);

    storage.SubmitBarrierGroup(desc);

    auto *header = FirstCommand(storage);

    auto *payload =
        GetPayloadAs<Nexus::Graphics::BarrierGroupCommandStorage, Nexus::Graphics::BufferBarrierCommandStorage>(header);

    EXPECT_EQ(payload[0].Offset, 256u);
    EXPECT_EQ(payload[0].Size, 4096u);
}