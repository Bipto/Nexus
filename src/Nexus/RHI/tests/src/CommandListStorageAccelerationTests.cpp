#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

class CommandListStorageAccelerationTests : public ::testing::Test
{
  protected:
    Nexus::Graphics::CommandListStorage Storage;
};

TEST_F(CommandListStorageAccelerationTests, BuildAccelerationStructuresWritesCommand)
{
    std::vector<Nexus::Graphics::AccelerationStructureGeometryBuildDescription> desc;
    desc.resize(2);

    Storage.BuildAccelerationStructures(desc);

    auto *header = FirstCommand(Storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::BuildAccelerationStructures);
}

TEST_F(CommandListStorageAccelerationTests, BuildAccelerationStructuresStoresCount)
{
    std::vector<Nexus::Graphics::AccelerationStructureGeometryBuildDescription> desc;
    desc.resize(5);

    Storage.BuildAccelerationStructures(desc);

    auto *header = FirstCommand(Storage);

    auto *payload = GetPayloadAs<
        Nexus::Graphics::AccelerationStructureGeometryBuildCommandStorage,
        Nexus::Graphics::AccelerationStructureGeometryBuildCommandStorage>(header);

    ASSERT_NE(payload, nullptr);
}