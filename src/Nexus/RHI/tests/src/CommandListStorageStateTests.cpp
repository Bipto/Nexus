#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageStateTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageStateTests, SetViewportStoresViewport)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::Viewport viewport{};
    viewport.X = 10.0f;
    viewport.Y = 20.0f;
    viewport.Width = 800.0f;
    viewport.Height = 600.0f;
    viewport.MinDepth = 0.1f;
    viewport.MaxDepth = 0.9f;

    storage.SetViewport(viewport);

    ASSERT_EQ(storage.CommandData.size(), sizeof(Nexus::Graphics::CommandHeader) + sizeof(Nexus::Graphics::Viewport));

    auto *header = FirstCommand(storage);
    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::Viewport);

    auto *cmd = GetCommand<Nexus::Graphics::Viewport>(header);

    EXPECT_FLOAT_EQ(cmd->X, viewport.X);
    EXPECT_FLOAT_EQ(cmd->Y, viewport.Y);
    EXPECT_FLOAT_EQ(cmd->Width, viewport.Width);
    EXPECT_FLOAT_EQ(cmd->Height, viewport.Height);
    EXPECT_FLOAT_EQ(cmd->MinDepth, viewport.MinDepth);
    EXPECT_FLOAT_EQ(cmd->MaxDepth, viewport.MaxDepth);
}

TEST(CommandListStorageStateTests, SetScissorStoresScissor)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::Scissor scissor{};
    scissor.X = 5;
    scissor.Y = 6;
    scissor.Width = 640;
    scissor.Height = 480;

    storage.SetScissor(scissor);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::Scissor);

    auto *cmd = GetCommand<Nexus::Graphics::Scissor>(header);

    EXPECT_EQ(cmd->X, scissor.X);
    EXPECT_EQ(cmd->Y, scissor.Y);
    EXPECT_EQ(cmd->Width, scissor.Width);
    EXPECT_EQ(cmd->Height, scissor.Height);
}

TEST(CommandListStorageStateTests, SetBlendFactorStoresStructure)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::BlendFactorDesc factor{};
    factor.Red = 0.1f;
    factor.Green = 0.2f;
    factor.Blue = 0.3f;
    factor.Alpha = 0.4f;

    storage.SetBlendFactor(factor);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::SetBlendFactor);

    auto *cmd = GetCommand<Nexus::Graphics::BlendFactorDesc>(header);

    EXPECT_FLOAT_EQ(cmd->Red, factor.Red);
    EXPECT_FLOAT_EQ(cmd->Green, factor.Green);
    EXPECT_FLOAT_EQ(cmd->Blue, factor.Blue);
    EXPECT_FLOAT_EQ(cmd->Alpha, factor.Alpha);
}

TEST(CommandListStorageStateTests, SetStencilReferenceStoresValue)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetStencilReference(42);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::SetStencilReference);

    auto *value = GetCommand<uint32_t>(header);

    EXPECT_EQ(*value, 42u);
}

TEST(CommandListStorageStateTests, SetFramebufferStoresHandleIndex)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::FramebufferHandle framebuffer(nullptr, Nexus::Graphics::FramebufferID(0, 0, nullptr));

    storage.SetFramebuffer(framebuffer);

    ASSERT_EQ(storage.Framebuffers.size(), 1u);
    EXPECT_EQ(storage.Framebuffers[0], framebuffer);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::FramebufferCommandStorage>(header);

    EXPECT_EQ(cmd->FramebufferIndex, 0u);
}

TEST(CommandListStorageStateTests, FramebufferIndicesIncrease)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::FramebufferHandle handle1(nullptr, Nexus::Graphics::FramebufferID(0, 0, nullptr));
    Nexus::Graphics::FramebufferHandle handle2(nullptr, Nexus::Graphics::FramebufferID(1, 1, nullptr));

    storage.SetFramebuffer(handle1);
    storage.SetFramebuffer(handle2);

    auto *first = FirstCommand(storage);
    auto *second = NextCommand(first);

    EXPECT_EQ(GetCommand<Nexus::Graphics::FramebufferCommandStorage>(first)->FramebufferIndex, 0u);
    EXPECT_EQ(GetCommand<Nexus::Graphics::FramebufferCommandStorage>(second)->FramebufferIndex, 1u);
}

TEST(CommandListStorageStateTests, SetPipelineStoresHandle)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::PipelineHandle handle1(nullptr, Nexus::Graphics::PipelineID(0, 0, nullptr));

    storage.SetPipeline(handle1);

    ASSERT_EQ(storage.Pipelines.size(), 1u);
    EXPECT_EQ(storage.Pipelines[0], handle1);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(header);

    EXPECT_EQ(cmd->PipelineIndex, 0u);
}

TEST(CommandListStorageStateTests, PipelineIndicesIncrease)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline(Nexus::Graphics::PipelineHandle{});
    storage.SetPipeline(Nexus::Graphics::PipelineHandle{});

    auto *first = FirstCommand(storage);
    auto *second = NextCommand(first);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(first)->PipelineIndex, 0u);
    EXPECT_EQ(GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(second)->PipelineIndex, 1u);
}

TEST(CommandListStorageStateTests, MixedCommandsRemainInCorrectOrder)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetPipeline({});
    storage.SetViewport({});
    storage.BeginDebugGroup("Frame");
    storage.Draw({});
    storage.InsertDebugMarker("Marker");
    storage.SetStencilReference(5);
    storage.EndDebugGroup();

    auto *c1 = FirstCommand(storage);
    auto *c2 = NextCommand(c1);
    auto *c3 = NextCommand(c2);
    auto *c4 = NextCommand(c3);
    auto *c5 = NextCommand(c4);
    auto *c6 = NextCommand(c5);
    auto *c7 = NextCommand(c6);

    EXPECT_EQ(c1->Type, Nexus::Graphics::CommandType::SetPipeline);
    EXPECT_EQ(c2->Type, Nexus::Graphics::CommandType::Viewport);
    EXPECT_EQ(c3->Type, Nexus::Graphics::CommandType::BeginDebugGroup);
    EXPECT_EQ(c4->Type, Nexus::Graphics::CommandType::Draw);
    EXPECT_EQ(c5->Type, Nexus::Graphics::CommandType::DebugLabel);
    EXPECT_EQ(c6->Type, Nexus::Graphics::CommandType::SetStencilReference);
    EXPECT_EQ(c7->Type, Nexus::Graphics::CommandType::EndDebugGroup);
}