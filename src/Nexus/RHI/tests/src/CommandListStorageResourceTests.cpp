#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageResourceTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST(CommandListStorageResourceTests, SetVertexBufferStoresBufferIndex)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::VertexBufferView view{};
    view.Offset = 128;
    view.Size = 4096;
    view.BufferHandle = {};

    storage.SetVertexBuffer(view, 3);

    ASSERT_EQ(storage.DeviceBuffers.size(), 1);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::SetVertexBuffer);

    auto *cmd = GetCommand<Nexus::Graphics::SetVertexBufferCommandStorage>(header);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0);
    EXPECT_EQ(cmd->Offset, 128);
    EXPECT_EQ(cmd->Size, 4096);
    EXPECT_EQ(cmd->Slot, 3);
}

TEST(CommandListStorageResourceTests, VertexBufferIndicesIncrease)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.SetVertexBuffer({}, 0);
    storage.SetVertexBuffer({}, 1);

    auto *first = FirstCommand(storage);
    auto *second = NextCommand(first);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetVertexBufferCommandStorage>(first)->DeviceBufferIndex, 0);

    EXPECT_EQ(GetCommand<Nexus::Graphics::SetVertexBufferCommandStorage>(second)->DeviceBufferIndex, 1);
}

TEST(CommandListStorageResourceTests, SetIndexBufferStoresFormat)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::IndexBufferView view{};
    view.Offset = 64;
    view.Size = 2048;
    view.BufferFormat = Nexus::Graphics::IndexFormat::UInt16;

    storage.SetIndexBuffer(view);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::SetIndexBufferCommandStorage>(header);

    EXPECT_EQ(cmd->DeviceBufferIndex, 0);
    EXPECT_EQ(cmd->Offset, 64);
    EXPECT_EQ(cmd->Size, 2048);
    EXPECT_EQ(cmd->BufferFormat, Nexus::Graphics::IndexFormat::UInt16);
}

TEST(CommandListStorageResourceTests, ResourceSetStoresDynamicOffsetCount)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::ResourceSetBindingDescription desc{};

    desc.DynamicOffsets = {{"Camera", {64}}, {"Material", {128}}};

    storage.SetResourceSet(desc);

    ASSERT_EQ(storage.ResourceSets.size(), 1);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::ResourceSetBindingCommandStorage>(header);

    EXPECT_EQ(cmd->ResourceSetIndex, 0);
    EXPECT_EQ(cmd->DynamicOffsetCount, 2);
}

TEST(CommandListStorageResourceTests, ResourceSetStoresPayload)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::ResourceSetBindingDescription desc{};

    desc.DynamicOffsets = {{"Camera", {64}}};

    storage.SetResourceSet(desc);

    auto *header = FirstCommand(storage);

    auto *cmd = GetCommand<Nexus::Graphics::ResourceSetBindingCommandStorage>(header);

    auto *payload = GetPayloadAs<Nexus::Graphics::ResourceSetBindingCommandStorage, char>(header);

    EXPECT_EQ(std::string(payload, 6), "Camera");

    uint32_t value = 0;

    memcpy(&value, payload + 6, sizeof(uint32_t));

    EXPECT_EQ(value, 64);
}

TEST(CommandListStorageResourceTests, EmptyResourceSetDoesNotWritePayload)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::ResourceSetBindingDescription desc{};

    storage.SetResourceSet(desc);

    ASSERT_EQ(storage.ResourceSets.size(), 1u);

    auto *cmd = GetCommand<Nexus::Graphics::ResourceSetBindingCommandStorage>(storage);

    EXPECT_EQ(cmd->DynamicOffsetCount, 0u);
}

TEST(CommandListStorageResourceTests, ResourceSetMultipleOffsetsPreserveOrder)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::ResourceSetBindingDescription desc{};

    desc.DynamicOffsets = {{"Camera", {16}}, {"Material", {32}}, {"Lights", {64}}};

    storage.SetResourceSet(desc);

    auto *cmd = GetCommand<Nexus::Graphics::ResourceSetBindingCommandStorage>(storage);

    EXPECT_EQ(cmd->DynamicOffsetCount, 3u);
}