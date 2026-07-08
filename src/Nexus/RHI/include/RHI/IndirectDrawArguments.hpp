#pragma once

#include "RHI/DeviceAddress.hpp"
#include <inttypes.h>

namespace Nexus::Graphics
{
    struct IndirectDrawArguments
    {
        uint32_t VertexCount = 0;
        uint32_t InstanceCount = 0;
        uint32_t FirstVertex = 0;
        uint32_t FirstInstance = 0;
    };

    struct IndirectIndexedDrawArguments
    {
        uint32_t IndexCount = 0;
        uint32_t InstanceCount = 0;
        uint32_t FirstIndex = 0;
        int32_t VertexOffset = 0;
        uint32_t FirstInstance = 0;
    };

    struct IndirectDispatchArguments
    {
        uint32_t GroupCountX = 0;
        uint32_t GroupCountY = 0;
        uint32_t GroupCountZ = 0;
    };

    struct IndirectMeshArguments
    {
        uint32_t GroupCountX = 0;
        uint32_t GroupCountY = 0;
        uint32_t GroupCountZ = 0;
    };

    struct IndirectTraceRaysArguments
    {
        DeviceAddressRegion RayGenerationShaderRecord = {};
        StridedDeviceAddressRegion MissShaderTable = {};
        StridedDeviceAddressRegion HitGroupTable = {};
        StridedDeviceAddressRegion CallableShaderTable = {};
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 0;
    };
} // namespace Nexus::Graphics