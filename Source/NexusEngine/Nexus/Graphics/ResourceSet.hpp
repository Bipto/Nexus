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
        std::vector<ResourceBinding> Resources;
    };

    class ResourceSet
    {
    public:
        ResourceSet(const ResourceSetSpecification &spec);
        virtual ~ResourceSet() {}

        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) = 0;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) = 0;

        const ResourceSetSpecification &GetSpecification() const;
        const std::unordered_map<uint32_t, uint32_t> &GetLinearBindings() const;

        static constexpr uint32_t DescriptorSlotCount = 64;
        static uint32_t GetLinearDescriptorSlot(uint32_t set, uint32_t slot);
        static std::unordered_map<uint32_t, uint32_t> RemapToLinearBindings(const ResourceSetSpecification &spec);

    protected:
        ResourceSetSpecification m_Specification;
        std::unordered_map<uint32_t, uint32_t> m_LinearBindings;
    };
}