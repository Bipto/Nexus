#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    ResourceSet::ResourceSet(const ResourceSetSpecification &spec)
        : m_Specification(spec)
    {
        if (HasUniformBuffers())
        {
            m_UniformBufferDescriptorIndex = 0;
            m_TextureDescriptorIndex = 1;
        }
        else
        {
            m_TextureDescriptorIndex = 0;
        }
    }

    const ResourceSetSpecification &ResourceSet::GetSpecification() const
    {
        return m_Specification;
    }

    bool ResourceSet::HasTextures() const
    {
        return m_Specification.TextureBindings.size() > 0;
    }

    bool ResourceSet::HasUniformBuffers() const
    {
        return m_Specification.UniformResourceBindings.size() > 0;
    }

    uint32_t ResourceSet::GetTextureDescriptorIndex() const
    {
        if (m_Specification.UniformResourceBindings.size() > 0)
            return 1;

        return 0;
    }

    uint32_t ResourceSet::GetUniformBufferDescriptorIndex() const
    {
        if (m_Specification.UniformResourceBindings.size() > 0)
            return 0;

        return 1;
    }

    uint32_t ResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t slot)
    {
        return set * ResourceSet::DescriptorSlotCount + slot;
    }
}