#pragma once

#include <string>
#include <inttypes.h>
#include <vector>

#include "Nexus/Types.hpp"
#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/Buffer.hpp"

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

        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) = 0;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) = 0;

        const std::map<uint32_t, uint32_t> &GetLinearTextureBindings() const;
        const std::map<uint32_t, uint32_t> &GetLinearUniformBufferBindings() const;
        const ResourceSetSpecification &GetSpecification() const;

        static constexpr uint32_t DescriptorSetCount = 4096;
        static uint32_t GetLinearDescriptorSlot(uint32_t set, uint32_t binding);

        static std::map<uint32_t, uint32_t> RemapToLinearBindings(const std::vector<ResourceBinding> &resources);

    protected:
        ResourceSetSpecification m_Specification;
        std::map<uint32_t, uint32_t> m_TextureBindings;
        std::map<uint32_t, uint32_t> m_UniformBufferBindings;
    };
}