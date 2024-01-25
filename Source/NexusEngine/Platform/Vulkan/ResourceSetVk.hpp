#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus/Graphics/ResourceSet.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class ResourceSetVk : public ResourceSet
    {
    public:
        ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device);
        ~ResourceSetVk();

        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) override;

        const std::map<uint32_t, VkDescriptorSetLayout> &GetDescriptorSetLayouts() const;
        const std::vector<std::map<uint32_t, VkDescriptorSet>> &GetDescriptorSets() const;

    private:
        VkDescriptorPool m_DescriptorPool;

        std::map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;
        std::vector<std::map<uint32_t, VkDescriptorSet>> m_DescriptorSets;

        GraphicsDeviceVk *m_Device;
    };
}

#endif