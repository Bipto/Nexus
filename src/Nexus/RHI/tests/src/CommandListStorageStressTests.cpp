#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageStressTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageStressTests, LargeCommandSequenceRemainsReadable)
{
    Nexus::Graphics::CommandListStorage storage;

    constexpr uint32_t Count = 1000;

    for (uint32_t i = 0; i < Count; i++)
    {
        storage.SetPipeline({});
        storage.SetStencilReference(i);
    }

    auto *command = FirstCommand(storage);

    for (uint32_t i = 0; i < Count; i++)
    {
        EXPECT_EQ(command->Type, Nexus::Graphics::CommandType::SetPipeline);

        command = NextCommand(command);

        EXPECT_EQ(command->Type, Nexus::Graphics::CommandType::SetStencilReference);

        auto *value = GetCommand<uint32_t>(command);

        EXPECT_EQ(*value, i);

        command = NextCommand(command);
    }
}