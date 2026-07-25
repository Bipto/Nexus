#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

TEST(CommandListStoragePushConstantsTests, StoresPushConstantMetadata)
{
    Nexus::Graphics::CommandListStorage storage;

    uint32_t value = 1234;

    storage.WritePushConstants("Transform", &value, sizeof(value), 16);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::PushConstants);

    auto *cmd = GetCommand<Nexus::Graphics::PushConstantsCommandStorage>(header);

    EXPECT_EQ(cmd->NameLength, 9);
    EXPECT_EQ(cmd->Offset, 16);
    EXPECT_EQ(cmd->DataLength, sizeof(value));
}

TEST(CommandListStoragePushConstantsTests, StoresPushConstantPayload)
{
    Nexus::Graphics::CommandListStorage storage;

    uint32_t value = 5678;

    storage.WritePushConstants("Value", &value, sizeof(value), 0);

    auto *header = FirstCommand(storage);

    auto *payload = GetPayloadAs<Nexus::Graphics::PushConstantsCommandStorage, std::byte>(header);

    EXPECT_EQ(std::string(reinterpret_cast<const char *>(payload), 5), "Value");

    uint32_t stored = 0;

    memcpy(&stored, payload + 5, sizeof(uint32_t));

    EXPECT_EQ(stored, value);
}

TEST(CommandListStoragePushConstantsTests, ZeroLengthPushConstants)
{
    Nexus::Graphics::CommandListStorage storage;

    storage.WritePushConstants("Empty", nullptr, 0, 16);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::PushConstants);

    auto *cmd = GetCommand<Nexus::Graphics::PushConstantsCommandStorage>(header);

    EXPECT_EQ(cmd->NameLength, 5u);
    EXPECT_EQ(cmd->Offset, 16u);
    EXPECT_EQ(cmd->DataLength, 0u);

    auto *payload = GetPayloadAs<Nexus::Graphics::PushConstantsCommandStorage, char>(header);

    EXPECT_EQ(std::string(payload, 5), "Empty");
}