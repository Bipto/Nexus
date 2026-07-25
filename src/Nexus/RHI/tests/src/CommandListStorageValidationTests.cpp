#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageValidationTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageValidationTests, EveryCommandHasValidLength)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetViewport({});
    storage.Draw({});
    storage.BeginDebugGroup("Test");
    storage.InsertDebugMarker("Marker");

    auto *command = FirstCommand(storage);

    while (command != nullptr)
    {
        EXPECT_GE(command->Length, sizeof(Nexus::Graphics::CommandHeader));

        command = NextCommand(command);
    }
}

TEST(CommandListStorageValidationTests, CommandStreamEndsExactlyAtBufferEnd)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.Draw({});
    storage.SetPipeline({});
    storage.Dispatch({});

    auto *command = FirstCommand(storage);

    size_t consumed = 0;

    while (command != nullptr)
    {
        consumed += command->Length;
        command = NextCommand(command);
    }

    EXPECT_EQ(consumed, storage.CommandData.size());
}

TEST(CommandListStorageValidationTests, ClearAfterPushConstants)
{
    Nexus::Graphics::CommandListStorage storage;

    uint32_t value = 42;

    storage.WritePushConstants("Test", &value, sizeof(value), 0);

    storage.Clear();

    EXPECT_TRUE(storage.CommandData.empty());

    storage.Draw({});

    EXPECT_EQ(FirstCommand(storage)->Type, Nexus::Graphics::CommandType::Draw);
}

TEST(CommandListStorageValidationTests, ResetClearsAllResourceTables)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetFramebuffer({});
    storage.SetVertexBuffer({}, 0);
    storage.SetIndexBuffer({});
    storage.CopyBufferToTexture({});
    storage.StartTimingQuery({});

    storage.Reset();

    EXPECT_TRUE(storage.Pipelines.empty());
    EXPECT_TRUE(storage.Framebuffers.empty());
    EXPECT_TRUE(storage.DeviceBuffers.empty());
    EXPECT_TRUE(storage.Textures.empty());
    EXPECT_TRUE(storage.TimingQueries.empty());
    EXPECT_TRUE(storage.CommandData.empty());
}

TEST(CommandListStorageValidationTests, CommandsSurviveCommandDataGrowth)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.Draw({});

    auto *first = FirstCommand(storage);

    for (size_t i = 0; i < 10000; i++)
    {
        storage.Draw({});
    }

    EXPECT_EQ(first->Type, Nexus::Graphics::CommandType::Draw);
}

TEST(CommandListStorageValidationTests, ResourceIndicesMatchStorage)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetPipeline({});
    storage.SetPipeline({});

    auto *cmd = FirstCommand(storage);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(cmd)->PipelineIndex, 0u);

    EXPECT_EQ(storage.Pipelines.size(), 3u);
}

TEST(CommandListStorageValidationTests, DefaultCommandsAreZeroInitialised)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.Draw({});
    storage.Dispatch({});
    storage.SetViewport({});

    auto *draw = GetCommand<Nexus::Graphics::DrawDescription>(FirstCommand(storage));

    EXPECT_EQ(draw->VertexCount, 0u);
}

TEST(CommandListStorageValidationTests, LargeDebugMarker)
{
    Nexus::Graphics::CommandListStorage storage;

    std::string text(4096, 'A');

    storage.InsertDebugMarker(text);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::DebugLabelCommandStorage>(header);

    EXPECT_EQ(cmd->TextLength, text.size());
}