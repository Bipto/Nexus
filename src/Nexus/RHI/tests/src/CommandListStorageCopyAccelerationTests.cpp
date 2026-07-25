#include <gtest/gtest.h>

#include "CommandListStorageTestHelpers.hpp"
#include "RHI/CommandList.hpp"

using namespace TestHelpers;

TEST(CommandListStorageCopyAccelerationTests, CopyAccelerationStructureStoresDescription)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::AccelerationStructureCopyDescription desc{};

    storage.CopyAccelerationStructure(desc);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::CopyAccelerationStructure);

    auto *stored = GetCommand<Nexus::Graphics::AccelerationStructureCopyDescription>(header);

    EXPECT_EQ(*stored, desc);
}

TEST(CommandListStorageCopyAccelerationTests, CopyAccelerationStructureToBufferUsesCorrectCommandType)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::AccelerationStructureDeviceBufferCopyDescription desc{};

    storage.CopyAccelerationStructureToDeviceBuffer(desc);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::CopyAccelerationStructureToDeviceBuffer);
}

TEST(CommandListStorageCopyAccelerationTests, CopyBufferToAccelerationStructureUsesCorrectCommandType)
{
    Nexus::Graphics::CommandListStorage storage;

    Nexus::Graphics::DeviceBufferAccelerationStructureCopyDescription desc{};

    storage.CopyDeviceBufferToAccelerationStructure(desc);

    auto *header = FirstCommand(storage);

    EXPECT_EQ(header->Type, Nexus::Graphics::CommandType::CopyDeviceBufferToAccelerationStructure);
}