#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    ResourceSet::ResourceSet(const ResourceSetSpecification &spec)
        : m_Specification(spec)
    {
        m_TextureBindings = RemapToLinearBindings(spec.Textures);
        m_UniformBufferBindings = RemapToLinearBindings(spec.UniformBuffers);

        for (const auto &texture : spec.Textures)
        {
            BindingInfo info;
            info.Set = texture.Set;
            info.Binding = texture.Binding;
            m_TextureBindingInfos[texture.Name] = info;
        }

        for (const auto &uniformBuffer : spec.UniformBuffers)
        {
            BindingInfo info;
            info.Set = uniformBuffer.Set;
            info.Binding = uniformBuffer.Binding;
            m_UniformBufferBindingInfos[uniformBuffer.Name] = info;
        }
    }

    const std::map<std::string, uint32_t> &ResourceSet::GetLinearTextureBindings() const
    {
        return m_TextureBindings;
    }

    const std::map<std::string, uint32_t> &ResourceSet::GetLinearUniformBufferBindings() const
    {
        return m_UniformBufferBindings;
    }

    const ResourceSetSpecification &ResourceSet::GetSpecification() const
    {
        return m_Specification;
    }

    uint32_t ResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t binding)
    {
        return (set * DescriptorSetCount) + binding;
    }

    std::map<std::string, uint32_t> ResourceSet::RemapToLinearBindings(const std::vector<ResourceBinding> &resources)
    {
        uint32_t resourceIndex = 0;
        std::map<std::string, uint32_t> bindings;

        for (const auto &resource : resources)
        {
            uint32_t remappedBinding = GetLinearDescriptorSlot(resource.Set, resource.Binding);
            bindings[resource.Name] = resourceIndex++;
        }

        return bindings;
    }
}