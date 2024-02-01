#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    ResourceSet::ResourceSet(const ResourceSetSpecification &spec)
        : m_Specification(spec)
    {
        m_LinearBindings = RemapToLinearBindings(spec);
    }

    const ResourceSetSpecification &ResourceSet::GetSpecification() const
    {
        return m_Specification;
    }

    const std::unordered_map<uint32_t, uint32_t> &ResourceSet::GetLinearBindings() const
    {
        return m_LinearBindings;
    }

    uint32_t ResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t slot)
    {
        return set * ResourceSet::DescriptorSlotCount + slot;
    }

    std::unordered_map<uint32_t, uint32_t> ResourceSet::RemapToLinearBindings(const ResourceSetSpecification &spec)
    {
        uint32_t textureIndex = 0;
        uint32_t uniformBufferIndex = 0;
        std::unordered_map<uint32_t, uint32_t> bindings;

        // TODO: should check if map already contains value

        for (const auto &binding : spec.Resources)
        {
            uint32_t remappedBinding = (binding.Set * ResourceSet::DescriptorSlotCount) + binding.Binding;
            if (bindings.find(remappedBinding) != bindings.end())
            {
                throw std::runtime_error("A resource has already been mapped to this slot");
            }

            if (binding.Type == ResourceType::CombinedImageSampler)
            {
                bindings[remappedBinding] = textureIndex++;
            }
            else if (binding.Type == ResourceType::UniformBuffer)
            {
                bindings[remappedBinding] = uniformBufferIndex++;
            }
            else
            {
                throw std::runtime_error("Failed to find a valid resource");
            }
        }

        return bindings;
    }
}