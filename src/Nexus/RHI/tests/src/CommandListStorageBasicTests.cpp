#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageTests, ClearRemovesAllRecordedState)
{
    Storage.Pipelines.emplace_back();
    Storage.DeviceBuffers.emplace_back();
    Storage.CommandData.resize(128);

    Storage.Clear();

    EXPECT_TRUE(Storage.Pipelines.empty());
    EXPECT_TRUE(Storage.DeviceBuffers.empty());
    EXPECT_TRUE(Storage.CommandData.empty());
}

TEST_F(CommandListStorageTests, ResetReturnsStorageToDefaultState)
{
    Storage.Pipelines.emplace_back();
    Storage.CommandData.resize(64);

    Storage.Reset();

    EXPECT_TRUE(Storage.Pipelines.empty());
    EXPECT_TRUE(Storage.DeviceBuffers.empty());
    EXPECT_TRUE(Storage.CommandData.empty());
    EXPECT_TRUE(Storage.Textures.empty());
    EXPECT_TRUE(Storage.ResourceSets.empty());
}

TEST_F(CommandListStorageTests, SetPipelineWritesPipelineCommand)
{
    Nexus::Graphics::PipelineHandle pipeline{};

    Storage.SetPipeline(pipeline);

    ASSERT_EQ(Storage.Pipelines.size(), 1u);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::SetPipeline);

    auto *cmd = GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(header);

    EXPECT_EQ(cmd->PipelineIndex, 0u);
}

TEST_F(CommandListStorageTests, PipelineIndicesIncrease)
{
    Storage.SetPipeline({});
    Storage.SetPipeline({});

    auto *first = FirstCommand(Storage);

    auto *second = NextCommand(first);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(first)->PipelineIndex, 0u);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetPipelineCommandStorage>(second)->PipelineIndex, 1u);
}

TEST_F(CommandListStorageTests, SetVertexBufferStoresCorrectValues)
{
    Nexus::Graphics::VertexBufferView view{};
    view.Offset = 64;
    view.Size = 1024;

    Storage.SetVertexBuffer(view, 5);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 1u);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::SetVertexBuffer);

    auto *cmd = GetCommand<Nexus::Graphics::SetVertexBufferCommandStorage>(header);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 64u);
    EXPECT_EQ(cmd->Size, 1024u);
    EXPECT_EQ(cmd->Slot, 5u);
}

TEST_F(CommandListStorageTests, SetIndexBufferStoresCorrectValues)
{
    Nexus::Graphics::IndexBufferView view{};
    view.Offset = 128;
    view.Size = 2048;
    view.BufferFormat = Nexus::Graphics::IndexFormat::UInt16;

    Storage.SetIndexBuffer(view);

    auto *cmd = GetCommand<Nexus::Graphics::SetIndexBufferCommandStorage>(Storage);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 128u);
    EXPECT_EQ(cmd->Size, 2048u);
    EXPECT_EQ(cmd->BufferFormat, Nexus::Graphics::IndexFormat::UInt16);
}

TEST_F(CommandListStorageTests, SetFramebufferStoresFramebufferIndex)
{
    Storage.SetFramebuffer({});

    ASSERT_EQ(Storage.Framebuffers.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::FramebufferCommandStorage>(Storage);

    EXPECT_EQ(cmd->FramebufferIndex, 0u);
}

TEST_F(CommandListStorageTests, ViewportIsCopied)
{
    Nexus::Graphics::Viewport vp{};
    vp.X = 1;
    vp.Y = 2;
    vp.Width = 640;
    vp.Height = 480;
    vp.MinDepth = 0.25f;
    vp.MaxDepth = 0.75f;

    Storage.SetViewport(vp);

    auto *stored = GetCommand<Nexus::Graphics::Viewport>(Storage);

    EXPECT_EQ(stored->X, vp.X);
    EXPECT_EQ(stored->Y, vp.Y);
    EXPECT_EQ(stored->Width, vp.Width);
    EXPECT_EQ(stored->Height, vp.Height);
    EXPECT_FLOAT_EQ(stored->MinDepth, vp.MinDepth);
    EXPECT_FLOAT_EQ(stored->MaxDepth, vp.MaxDepth);
}

TEST_F(CommandListStorageTests, ScissorIsCopied)
{
    Nexus::Graphics::Scissor s{};
    s.X = 10;
    s.Y = 20;
    s.Width = 800;
    s.Height = 600;

    Storage.SetScissor(s);

    auto *stored = GetCommand<Nexus::Graphics::Scissor>(Storage);

    EXPECT_EQ(stored->X, s.X);
    EXPECT_EQ(stored->Y, s.Y);
    EXPECT_EQ(stored->Width, s.Width);
    EXPECT_EQ(stored->Height, s.Height);
}

TEST_F(CommandListStorageTests, BlendFactorCopiedVerbatim)
{
    Nexus::Graphics::BlendFactorDesc factor{};
    factor.Red = 0.2f;
    factor.Green = 0.3f;
    factor.Blue = 0.4f;
    factor.Alpha = 0.5f;

    Storage.SetBlendFactor(factor);

    auto *stored = GetCommand<Nexus::Graphics::BlendFactorDesc>(Storage);

    EXPECT_FLOAT_EQ(stored->Red, factor.Red);
    EXPECT_FLOAT_EQ(stored->Green, factor.Green);
    EXPECT_FLOAT_EQ(stored->Blue, factor.Blue);
    EXPECT_FLOAT_EQ(stored->Alpha, factor.Alpha);
}

TEST_F(CommandListStorageTests, StencilReferenceStored)
{
    Storage.SetStencilReference(42);

    auto *value = GetCommand<uint32_t>(Storage);

    EXPECT_EQ(*value, 42u);
}

TEST_F(CommandListStorageTests, CommandsRemainSequential)
{
    Storage.SetPipeline({});
    Storage.SetFramebuffer({});
    Storage.SetStencilReference(7);

    auto *c1 = FirstCommand(Storage);
    auto *c2 = NextCommand(c1);
    auto *c3 = NextCommand(c2);

    EXPECT_EQ(c1->Type, Nexus::Graphics::CommandType::SetPipeline);

    EXPECT_EQ(c2->Type, Nexus::Graphics::CommandType::SetFramebuffer);

    EXPECT_EQ(c3->Type, Nexus::Graphics::CommandType::SetStencilReference);
}