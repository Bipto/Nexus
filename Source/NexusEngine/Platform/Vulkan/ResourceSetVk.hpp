#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Nexus/Graphics/ResourceSet.hpp"
#include "GraphicsDeviceVk.hpp"
#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    class ResourceSetVk : public ResourceSet
    {
    public:
        ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device);
        ~ResourceSetVk();

        virtual void PerformResourceUpdate() override;
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;

        const std::map<uint32_t, VkDescriptorSetLayout> &GetDescriptorSetLayouts() const;
        const std::vector<std::map<uint32_t, VkDescriptorSet>> &GetDescriptorSets() const;

    private:
        VkDescriptorPool m_DescriptorPool;

        std::map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;
        std::vector<std::map<uint32_t, VkDescriptorSet>> m_DescriptorSets;

        GraphicsDeviceVk *m_Device = nullptr;
    };
}

#endif