#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageStreamTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageStreamTests, MixedCommandsRemainInOrder)
{
    uint32_t value = 123;

    Storage.SetPipeline({});
    Storage.SetViewport({});
    Storage.WritePushConstants("Value", &value, sizeof(value), 0);
    Storage.Draw({});
    Storage.SetStencilReference(5);

    auto *c1 = FirstCommand(Storage);
    auto *c2 = NextCommand(c1);
    auto *c3 = NextCommand(c2);
    auto *c4 = NextCommand(c3);
    auto *c5 = NextCommand(c4);

    EXPECT_EQ(c1->Type, Nexus::Graphics::CommandType::SetPipeline);

    EXPECT_EQ(c2->Type, Nexus::Graphics::CommandType::Viewport);

    EXPECT_EQ(c3->Type, Nexus::Graphics::CommandType::PushConstants);

    EXPECT_EQ(c4->Type, Nexus::Graphics::CommandType::Draw);

    EXPECT_EQ(c5->Type, Nexus::Graphics::CommandType::SetStencilReference);
}

TEST_F(CommandListStorageStreamTests, VariableLengthCommandDoesNotCorruptNextCommand)
{
    uint32_t value = 999;

    Storage.WritePushConstants("Transform", &value, sizeof(value), 64);

    Storage.Draw({});

    auto *push = FirstCommand(Storage);
    auto *draw = NextCommand(push);

    ASSERT_NE(draw, nullptr);

    EXPECT_EQ(push->Type, Nexus::Graphics::CommandType::PushConstants);

    EXPECT_EQ(draw->Type, Nexus::Graphics::CommandType::Draw);
}

TEST_F(CommandListStorageStreamTests, ManyCommandsCanBeWalked)
{
    constexpr size_t count = 1000;

    for (size_t i = 0; i < count; i++)
    {
        Storage.Draw({});
    }

    Nexus::Graphics::CommandIterator iterator(Storage.CommandData);

    size_t visited = 0;

    for (auto iterator = Storage.GetCommands(); iterator.HasNext(); iterator.Next())
    {
        auto *command = iterator.Get();
        EXPECT_EQ(command->Type, Nexus::Graphics::CommandType::Draw);
        visited++;
    }

    EXPECT_EQ(visited, count);
}