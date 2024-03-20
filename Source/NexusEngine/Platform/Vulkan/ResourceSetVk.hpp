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

        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;
        virtual void Flush() override;

        const std::map<uint32_t, VkDescriptorSetLayout> &GetDescriptorSetLayouts() const;
        const std::vector<std::map<uint32_t, VkDescriptorSet>> &GetDescriptorSets() const;

    private:
        VkDescriptorPool m_DescriptorPool;

        std::map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;
        std::vector<std::map<uint32_t, VkDescriptorSet>> m_DescriptorSets;

        std::map<std::string, std::pair<Ref<Texture>, Ref<Sampler>>> m_BoundCombinedImageSamplers;
        std::map<std::string, Ref<UniformBuffer>> m_BoundUniformBuffers;

        GraphicsDeviceVk *m_Device = nullptr;
    };
}

#endif