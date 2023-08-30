#pragma once

#include "Vk.hpp"
#include "Core/Graphics/ResourceSet.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class ResourceSetVk : public ResourceSet
    {
    public:
        ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device);
        virtual void UpdateTexture(Ref<Texture> texture, uint32_t binding) override;

    private:
        VkDescriptorSetLayout m_SamplerLayout;
        VkDescriptorSetLayout m_UniformBufferLayout;
        VkDescriptorPool m_DescriptorPool;
    };
}