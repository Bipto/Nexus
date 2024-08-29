#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
ResourceSet::ResourceSet(const ResourceSetSpecification &spec) : m_Specification(spec)
{

    for (const auto &texture : spec.SampledImages)
    {
        BindingInfo info;
        info.Set = texture.Set;
        info.Binding = texture.Binding;
        m_CombinedImageSamplerBindingInfos[texture.Name] = info;
    }

    for (const auto &uniformBuffer : spec.UniformBuffers)
    {
        BindingInfo info;
        info.Set = uniformBuffer.Set;
        info.Binding = uniformBuffer.Binding;
        m_UniformBufferBindingInfos[uniformBuffer.Name] = info;
    }
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

const std::map<std::string, WeakRef<UniformBuffer>> &ResourceSet::GetBoundUniformBuffers() const
{
    return m_BoundUniformBuffers;
}

const std::map<std::string, CombinedImageSampler> &ResourceSet::GetBoundCombinedImageSamplers() const
{
    return m_BoundCombinedImageSamplers;
}
} // namespace Nexus::Graphics