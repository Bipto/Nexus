#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageDrawTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageDrawTests, DrawCopiesDescription)
{
    Nexus::Graphics::DrawDescription draw{};
    draw.VertexCount = 100;
    draw.InstanceCount = 5;
    draw.VertexStart = 10;
    draw.InstanceStart = 2;

    Storage.Draw(draw);

    auto *header = FirstCommand(Storage);

    ASSERT_EQ(header->Type, Nexus::Graphics::CommandType::Draw);

    auto *stored = GetCommand<Nexus::Graphics::DrawDescription>(header);

    EXPECT_EQ(stored->VertexCount, draw.VertexCount);
    EXPECT_EQ(stored->InstanceCount, draw.InstanceCount);
    EXPECT_EQ(stored->VertexStart, draw.VertexStart);
    EXPECT_EQ(stored->InstanceStart, draw.InstanceStart);
}

TEST_F(CommandListStorageDrawTests, DrawIndexedCopiesDescription)
{
    Nexus::Graphics::DrawIndexedDescription draw{};
    draw.IndexCount = 500;
    draw.InstanceCount = 3;
    draw.IndexStart = 7;
    draw.VertexStart = -2;
    draw.InstanceStart = 11;

    Storage.DrawIndexed(draw);

    auto *stored = GetCommand<Nexus::Graphics::DrawIndexedDescription>(Storage);

    EXPECT_EQ(stored->IndexCount, draw.IndexCount);
    EXPECT_EQ(stored->InstanceCount, draw.InstanceCount);
    EXPECT_EQ(stored->IndexStart, draw.IndexStart);
    EXPECT_EQ(stored->VertexStart, draw.VertexStart);
    EXPECT_EQ(stored->InstanceStart, draw.InstanceStart);
}

TEST_F(CommandListStorageDrawTests, DrawIndirectStoresBufferIndex)
{
    Nexus::Graphics::DrawIndirectDescription desc{};

    desc.Offset = 64;
    desc.Stride = 32;
    desc.DrawCount = 8;

    Storage.DrawIndirect(desc);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::DrawIndirectCommandStorage>(Storage);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 64u);
    EXPECT_EQ(cmd->Stride, 32u);
    EXPECT_EQ(cmd->DrawCount, 8u);
}

TEST_F(CommandListStorageDrawTests, DrawIndexedIndirectUsesSameStorage)
{
    Nexus::Graphics::DrawIndirectIndexedDescription desc{};

    desc.Offset = 128;
    desc.Stride = 64;
    desc.DrawCount = 16;

    Storage.DrawIndexedIndirect(desc);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::DrawIndexedIndirect);

    auto *cmd = GetCommand<Nexus::Graphics::DrawIndirectCommandStorage>(header);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 128u);
    EXPECT_EQ(cmd->Stride, 64u);
    EXPECT_EQ(cmd->DrawCount, 16u);
}

TEST_F(CommandListStorageDrawTests, DispatchCopiesDescription)
{
    Nexus::Graphics::DispatchDescription dispatch{};

    dispatch.WorkGroupCountX = 8;
    dispatch.WorkGroupCountY = 4;
    dispatch.WorkGroupCountZ = 2;

    Storage.Dispatch(dispatch);

    auto *stored = GetCommand<Nexus::Graphics::DispatchDescription>(Storage);

    EXPECT_EQ(stored->WorkGroupCountX, 8u);
    EXPECT_EQ(stored->WorkGroupCountY, 4u);
    EXPECT_EQ(stored->WorkGroupCountZ, 2u);
}

TEST_F(CommandListStorageDrawTests, DispatchIndirectStoresBuffer)
{
    Nexus::Graphics::DispatchIndirectDescription dispatch{};

    dispatch.Offset = 512;
    dispatch.Stride = 24;

    Storage.DispatchIndirect(dispatch);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::DispatchIndirectCommandStorage>(Storage);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 512u);
    EXPECT_EQ(cmd->Stride, 24u);
}

TEST_F(CommandListStorageDrawTests, DrawMeshCopiesDescription)
{
    Nexus::Graphics::DrawMeshDescription mesh{};

    mesh.WorkGroupCountX = 32;
    mesh.WorkGroupCountY = 8;
    mesh.WorkGroupCountZ = 1;

    Storage.DrawMesh(mesh);

    auto *stored = GetCommand<Nexus::Graphics::DrawMeshDescription>(Storage);

    EXPECT_EQ(stored->WorkGroupCountX, mesh.WorkGroupCountX);

    EXPECT_EQ(stored->WorkGroupCountY, mesh.WorkGroupCountY);

    EXPECT_EQ(stored->WorkGroupCountZ, mesh.WorkGroupCountZ);
}

TEST_F(CommandListStorageDrawTests, DrawMeshIndirectStoresBuffer)
{
    Nexus::Graphics::DrawMeshIndirectDescription mesh{};

    mesh.Offset = 1024;
    mesh.Stride = 48;
    mesh.DrawCount = 9;

    Storage.DrawMeshIndirect(mesh);

    ASSERT_EQ(Storage.DeviceBuffers.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::DrawMeshIndirectCommandStorage>(Storage);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0u);
    EXPECT_EQ(cmd->Offset, 1024u);
    EXPECT_EQ(cmd->Stride, 48u);
    EXPECT_EQ(cmd->DrawCount, 9u);
}

TEST_F(CommandListStorageDrawTests, TraceRaysCopiesDescription)
{
    Nexus::Graphics::TraceRaysDescription desc{};

    desc.Width = 1280;
    desc.Height = 720;
    desc.Depth = 4;

    Storage.TraceRays(desc);

    auto *stored = GetCommand<Nexus::Graphics::TraceRaysDescription>(Storage);

    EXPECT_EQ(stored->Width, desc.Width);
    EXPECT_EQ(stored->Height, desc.Height);
    EXPECT_EQ(stored->Depth, desc.Depth);

    EXPECT_EQ(stored->RaygenRegion.Address, desc.RaygenRegion.Address);
    EXPECT_EQ(stored->RaygenRegion.Size, desc.RaygenRegion.Size);

    EXPECT_EQ(stored->MissRegion.Address, desc.MissRegion.Address);
    EXPECT_EQ(stored->MissRegion.Size, desc.MissRegion.Size);

    EXPECT_EQ(stored->HitRegion.Address, desc.HitRegion.Address);
    EXPECT_EQ(stored->HitRegion.Size, desc.HitRegion.Size);

    EXPECT_EQ(stored->CallableRegion.Address, desc.CallableRegion.Address);
    EXPECT_EQ(stored->CallableRegion.Size, desc.CallableRegion.Size);
}

TEST_F(CommandListStorageDrawTests, IndirectCommandsIncrementBufferIndices)
{
    Nexus::Graphics::DrawIndirectDescription draw{};
    Nexus::Graphics::DispatchIndirectDescription dispatch{};
    Nexus::Graphics::DrawMeshIndirectDescription mesh{};

    Storage.DrawIndirect(draw);
    Storage.DispatchIndirect(dispatch);
    Storage.DrawMeshIndirect(mesh);

    auto *first = FirstCommand(Storage);
    auto *second = NextCommand(first);
    auto *third = NextCommand(second);

    EXPECT_EQ(GetCommand<Nexus::Graphics::DrawIndirectCommandStorage>(first)->DeviceBufferIndex, 0u);

    EXPECT_EQ(GetCommand<Nexus::Graphics::DispatchIndirectCommandStorage>(second)->DeviceBufferIndex, 1u);

    EXPECT_EQ(GetCommand<Nexus::Graphics::DrawMeshIndirectCommandStorage>(third)->DeviceBufferIndex, 2u);

    EXPECT_EQ(Storage.DeviceBuffers.size(), 3u);
}

TEST_F(CommandListStorageDrawTests, DefaultDrawDescriptionSerialises)
{
    Nexus::Graphics::DrawDescription draw{};

    Storage.Draw(draw);

    auto *stored = GetCommand<Nexus::Graphics::DrawDescription>(Storage);

    EXPECT_EQ(0u, stored->VertexCount);
    EXPECT_EQ(0u, stored->InstanceCount);
    EXPECT_EQ(0u, stored->VertexStart);
    EXPECT_EQ(0u, stored->InstanceStart);
}