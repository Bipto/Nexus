#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageLifecycleTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageLifecycleTests, ClearRemovesMixedResources)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetFramebuffer({});
    storage.SetVertexBuffer({}, 0);
    storage.CopyTextureToTexture({});
    storage.StartTimingQuery({});

    storage.Clear();

    EXPECT_TRUE(storage.Pipelines.empty());
    EXPECT_TRUE(storage.Framebuffers.empty());
    EXPECT_TRUE(storage.DeviceBuffers.empty());
    EXPECT_TRUE(storage.Textures.empty());
    EXPECT_TRUE(storage.TimingQueries.empty());
    EXPECT_TRUE(storage.CommandData.empty());
}