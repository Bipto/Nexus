#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    ResourceSet::ResourceSet(const ResourceSetSpecification &spec)
        : m_Specification(spec)
    {
        m_TextureBindings = RemapToLinearBindings(spec.Textures);
        m_UniformBufferBindings = RemapToLinearBindings(spec.UniformBuffers);
    }

    const std::unordered_map<uint32_t, uint32_t> &ResourceSet::GetLinearTextureBindings() const
    {
        return m_TextureBindings;
    }

    const std::unordered_map<uint32_t, uint32_t> &ResourceSet::GetLinearUniformBufferBindings() const
    {
        return m_UniformBufferBindings;
    }

    const ResourceSetSpecification &ResourceSet::GetSpecification() const
    {
        return m_Specification;
    }

    uint32_t ResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t slot)
    {
        return set * ResourceSet::DescriptorSlotCount + slot;
    }

    std::unordered_map<uint32_t, uint32_t> ResourceSet::RemapToLinearBindings(const std::vector<ResourceBinding> &resources)
    {
        uint32_t resourceIndex = 0;
        std::unordered_map<uint32_t, uint32_t> bindings;

        for (const auto &resource : resources)
        {
            uint32_t remappedBinding = ResourceSet::GetLinearDescriptorSlot(resource.Set, resource.Binding);

            if (bindings.find(remappedBinding) != bindings.end())
            {
                throw std::runtime_error("A resource has already been mapped to this slot");
            }

            bindings[remappedBinding] = resourceIndex++;
        }

        return bindings;
    }
}