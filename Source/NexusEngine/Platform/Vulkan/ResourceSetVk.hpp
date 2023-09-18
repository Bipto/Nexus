#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Core/Graphics/ResourceSet.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class ResourceSetVk : public ResourceSet
    {
    public:
        ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device);
        ~ResourceSetVk();

        virtual void WriteTexture(Ref<Texture> texture, uint32_t binding) override;
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding) override;

        VkDescriptorSetLayout GetSamplerDescriptorSetLayout();
        VkDescriptorSet GetSamplerDescriptorSet();
        VkDescriptorSetLayout GetUniformBufferDescriptorSetLayout();
        VkDescriptorSet GetUniformBufferrDescriptorSet();

    private:
        std::vector<VkDescriptorSetLayout> m_SamplerLayout;
        std::vector<VkDescriptorSetLayout> m_UniformBufferLayout;
        VkDescriptorPool m_DescriptorPool;

        std::vector<VkDescriptorSet> m_SamplerDescriptorSet;
        std::vector<VkDescriptorSet> m_UniformBufferDescriptorSet;

        GraphicsDeviceVk *m_Device;
    };
}

#endif