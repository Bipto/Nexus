#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageResetTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};
TEST(CommandListStorageResetTests, ClearRemovesAllCommands)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetFramebuffer({});
    storage.SetStencilReference(5);

    storage.Clear();

    EXPECT_TRUE(storage.CommandData.empty());
    EXPECT_TRUE(storage.Pipelines.empty());
    EXPECT_TRUE(storage.Framebuffers.empty());
}

TEST(CommandListStorageResetTests, ResetRestoresDefaultState)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetViewport({});

    storage.Reset();

    EXPECT_TRUE(storage.CommandData.empty());
    EXPECT_TRUE(storage.Pipelines.empty());
    EXPECT_TRUE(storage.DeviceBuffers.empty());
    EXPECT_TRUE(storage.Textures.empty());
}