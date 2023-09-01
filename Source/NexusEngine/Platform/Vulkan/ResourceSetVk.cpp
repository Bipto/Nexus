#if defined(NX_PLATFORM_VULKAN)

#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
    ResourceSetVk::ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device)
    {
        // allocating uniform buffer descriptor set
        {
            std::vector<VkDescriptorSetLayoutBinding> uniformBufferBindings;
            for (const auto &item : spec.UniformResourceBindings)
            {
                VkDescriptorSetLayoutBinding uniformBufferBinding;
                uniformBufferBinding.binding = item.Binding;
                uniformBufferBinding.descriptorCount = 1;
                uniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                uniformBufferBindings.push_back(uniformBufferBinding);
            }

            VkDescriptorSetLayoutCreateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.pNext = nullptr;
            setInfo.flags = 0;
            setInfo.bindingCount = uniformBufferBindings.size();
            setInfo.pBindings = uniformBufferBindings.data();

            if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &m_UniformBufferLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor set layout");
            }
        }

        // allocating sampler descriptor set
        {
            std::vector<VkDescriptorSetLayoutBinding> textureBindings;
            for (const auto &item : spec.TextureBindings)
            {
                VkDescriptorSetLayoutBinding samplerBinding = {};
                samplerBinding.binding = item.Slot;
                samplerBinding.descriptorCount = 1;
                samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                textureBindings.push_back(samplerBinding);
            }

            VkDescriptorSetLayoutCreateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.pNext = nullptr;
            setInfo.flags = 0;
            setInfo.bindingCount = textureBindings.size();
            setInfo.pBindings = textureBindings.data();

            if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &m_SamplerLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor set layout");
            }
        }

        // allocate descriptor pool
        {
            std::vector<VkDescriptorPoolSize> sizes;

            if (spec.UniformResourceBindings.size() > 0)
            {
                VkDescriptorPoolSize size;
                size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                size.descriptorCount = spec.UniformResourceBindings.size();
            }

            if (spec.TextureBindings.size() > 0)
            {
                VkDescriptorPoolSize size;
                size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                size.descriptorCount = spec.TextureBindings.size();
            }

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = 0;
            poolInfo.maxSets = spec.TextureBindings.size() + spec.UniformResourceBindings.size();
            poolInfo.poolSizeCount = (uint32_t)sizes.size();
            poolInfo.pPoolSizes = sizes.data();

            if (vkCreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor pool");
            }
        }
    }

    void ResourceSetVk::UpdateTexture(Ref<Texture> texture, uint32_t binding)
    {
    }
}

#endif