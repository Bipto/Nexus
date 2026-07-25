#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

TEST(CommandListStorageBarrierTests, EmptyBarrierGroupStoresZeroCounts)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::BarrierGroupDescription desc{};

    storage.SubmitBarrierGroup(desc);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::BarrierGroup);

    auto *cmd = GetCommand<Nexus::Graphics::BarrierGroupCommandStorage>(header);

    EXPECT_EQ(cmd->MemoryBarrierCount, 0u);
    EXPECT_EQ(cmd->TextureBarrierCount, 0u);
    EXPECT_EQ(cmd->BufferBarrierCount, 0u);
}

TEST(CommandListStorageBarrierTests, MixedBarrierGroupStoresCounts)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::BarrierGroupDescription desc{};

    desc.TextureBarriers.resize(2);
    desc.BufferBarriers.resize(3);

    storage.SubmitBarrierGroup(desc);

    auto *cmd = GetCommand<Nexus::Graphics::BarrierGroupCommandStorage>(storage);

    EXPECT_EQ(cmd->TextureBarrierCount, 2u);
    EXPECT_EQ(cmd->BufferBarrierCount, 3u);
}