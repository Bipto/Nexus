#pragma once

#include <string>
#include <inttypes.h>
#include <vector>

#include "Nexus/Types.hpp"
#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/Buffer.hpp"
#include "Nexus/Graphics/Sampler.hpp"

namespace Nexus::Graphics
{
    enum class ResourceType
    {
        CombinedImageSampler,
        UniformBuffer
    };

    struct ResourceBinding
    {
        std::string Name;
        uint32_t Set;
        uint32_t Binding;
        ResourceType Type;
    };

    struct BindingInfo
    {
        uint32_t Set = 0;
        uint32_t Binding = 0;
    };

    struct ResourceSetSpecification
    {
        std::vector<ResourceBinding> Textures;
        std::vector<ResourceBinding> UniformBuffers;
    };

    class ResourceSet
    {
    public:
        ResourceSet(const ResourceSetSpecification &spec);
        virtual ~ResourceSet() {}

        virtual void PerformResourceUpdate() = 0;
        virtual void WriteTexture(Texture *texture, const std::string &name) = 0;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name) = 0;
        virtual void WriteSampler(Sampler *sampler, const std::string &name) = 0;
        virtual void WriteCombinedImageSampler(Texture *texture, Sampler *sampler, const std::string &name) = 0;

        const ResourceSetSpecification &GetSpecification() const;
        static constexpr uint32_t DescriptorSetCount = 64;
        static uint32_t GetLinearDescriptorSlot(uint32_t set, uint32_t binding);

        static std::map<std::string, uint32_t> RemapToLinearBindings(const std::vector<ResourceBinding> &resources);

    protected:
        ResourceSetSpecification m_Specification;
        std::map<std::string, BindingInfo> m_TextureBindingInfos;
        std::map<std::string, BindingInfo> m_UniformBufferBindingInfos;
        std::map<std::string, BindingInfo> m_SamplerBindingInfos;
    };
}