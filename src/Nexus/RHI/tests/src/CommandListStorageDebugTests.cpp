#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageDebugTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageDebugTests, BeginDebugGroupStoresName)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.BeginDebugGroup("ShadowPass");

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::BeginDebugGroup);

    auto *size = GetCommand<size_t>(header);

    EXPECT_EQ(*size, 10);

    auto text = PayloadString(header, sizeof(size_t), alignof(size_t), 10);

    EXPECT_EQ(text, "ShadowPass");
}

TEST(CommandListStorageDebugTests, InsertDebugMarkerStoresText)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.InsertDebugMarker("Draw Terrain");

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::DebugLabel);

    auto *cmd = GetCommand<Nexus::Graphics::DebugLabelCommandStorage>(header);

    EXPECT_EQ(cmd->TextLength, 12);

    EXPECT_EQ(
        PayloadString(
            header, sizeof(Nexus::Graphics::DebugLabelCommandStorage),
            alignof(Nexus::Graphics::DebugLabelCommandStorage), cmd->TextLength
        ),
        "Draw Terrain"
    );
}

TEST(CommandListStorageDebugTests, EmptyDebugGroupIsSupported)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.BeginDebugGroup("");

    auto *header = FirstCommand(storage);

    auto *size = GetCommand<size_t>(header);

    EXPECT_EQ(*size, 0u);
}

TEST(CommandListStorageDebugTests, DebugCommandsRemainOrdered)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.BeginDebugGroup("Pass");
    storage.InsertDebugMarker("Draw");
    storage.EndDebugGroup();

    auto *first = FirstCommand(storage);
    auto *second = NextCommand(first);
    auto *third = NextCommand(second);

    EXPECT_EQ(first->Type, Nexus::Graphics::CommandType::BeginDebugGroup);

    EXPECT_EQ(second->Type, Nexus::Graphics::CommandType::DebugLabel);

    EXPECT_EQ(third->Type, Nexus::Graphics::CommandType::EndDebugGroup);
}