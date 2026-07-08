#pragma once

#include "RHI/ShaderDataType.hpp"
#include "RHI/ShaderResources.hpp"

namespace Nexus::Graphics
{
    struct ReflectedResource
    {
        ReflectedShaderDataType Type = ReflectedShaderDataType::UniformBuffer;
        std::string BlockName = {};
        std::string InstanceName = {};
        ResourceDimension Dimension = ResourceDimension::NoDimension;
        StorageResourceAccess ResourceAccess = StorageResourceAccess::NoAccess;
        uint32_t DescriptorSet = 0;
        uint32_t BindingPoint = 0;
        uint32_t BindingCount = 0;
        uint32_t RegisterSpace = 0;
    };

    struct Attribute
    {
        std::string Name = {};
        ReflectedShaderDataType Type = {};
        uint32_t Binding = {};
        uint32_t StreamIndex = {};
    };

    struct ReflectedBufferMember
    {
        std::string Name = {};
        size_t Offset = 0;
        size_t Size = 0;
        std::optional<uint32_t> ArraySize = {};
        ShaderDataType Type = {};
    };

    struct ReflectedUniformBuffer
    {
        std::string Name = {};
        std::vector<ReflectedBufferMember> Members = {};
    };

    struct ReflectedStorageBuffer
    {
        std::string Name = {};
        std::vector<ReflectedBufferMember> Members = {};
    };

    struct ShaderReflectionData
    {
        std::vector<Attribute> Inputs;
        std::vector<Attribute> Outputs;
        std::vector<ReflectedUniformBuffer> UniformBuffers = {};
        std::vector<ReflectedStorageBuffer> StorageBuffers = {};
        std::vector<ReflectedResource> Resources = {};
    };

    struct ShaderAttribute
    {
        std::string Name;
        ShaderDataType DataType;
    };
} // namespace Nexus::Graphics