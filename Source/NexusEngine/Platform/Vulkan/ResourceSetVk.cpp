#if defined(NX_PLATFORM_VULKAN)

#include "ResourceSetVk.hpp"
#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    ResourceSetVk::ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device)
        : m_Device(device)
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

            if (uniformBufferBindings.size() > 0)
            {
                setInfo.bindingCount = uniformBufferBindings.size();
                setInfo.pBindings = uniformBufferBindings.data();
            }
            else
            {
                setInfo.bindingCount = 0;
                setInfo.pBindings = nullptr;
            }

            m_UniformBufferLayout.resize(FRAMES_IN_FLIGHT);

            for (int i = 0; i < m_UniformBufferLayout.size(); i++)
            {
                if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &m_UniformBufferLayout[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create descriptor set layout");
                }
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

            if (textureBindings.size() > 0)
            {
                setInfo.bindingCount = textureBindings.size();
                setInfo.pBindings = textureBindings.data();
            }
            else
            {
                setInfo.bindingCount = 0;
                setInfo.pBindings = nullptr;
            }

            m_SamplerLayout.resize(FRAMES_IN_FLIGHT);

            for (int i = 0; i < m_SamplerLayout.size(); i++)
            {
                if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &m_SamplerLayout[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create descriptor set layout");
                }
            }
        }

        // allocate descriptor pool
        {
            std::vector<VkDescriptorPoolSize> sizes;

            // set up pool for uniform buffers
            if (spec.UniformResourceBindings.size() > 0)
            {
                VkDescriptorPoolSize size;
                size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                int descriptorCount = std::clamp((int)spec.UniformResourceBindings.size(), 1, 1000);
                size.descriptorCount = descriptorCount;
                sizes.push_back(size);
            }

            // set up pool for samplers
            if (spec.TextureBindings.size() > 0)
            {
                VkDescriptorPoolSize size;
                size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                int descriptorCount = std::clamp((int)spec.TextureBindings.size(), 1, 1000);
                size.descriptorCount = descriptorCount;
                sizes.push_back(size);
            }

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = 0;

            // allow 2 descriptor sets to be allocated for each frame
            poolInfo.maxSets = FRAMES_IN_FLIGHT * 2;
            poolInfo.poolSizeCount = (uint32_t)sizes.size();
            poolInfo.pPoolSizes = sizes.data();

            if (vkCreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor pool");
            }
        }

        // allocate uniform buffer descriptor set
        {
            m_UniformBufferDescriptorSet.resize(FRAMES_IN_FLIGHT);

            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.pNext = nullptr;
            allocInfo.descriptorPool = m_DescriptorPool;
            allocInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
            allocInfo.pSetLayouts = m_UniformBufferLayout.data();

            if (vkAllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, m_UniformBufferDescriptorSet.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate uniform buffer descriptor set");
            }
        }

        // allocate descriptor set
        {
            m_SamplerDescriptorSet.resize(FRAMES_IN_FLIGHT);

            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.pNext = nullptr;
            allocInfo.descriptorPool = m_DescriptorPool;
            allocInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
            allocInfo.pSetLayouts = m_SamplerLayout.data();

            if (vkAllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, m_SamplerDescriptorSet.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate descriptor sets");
            }
        }
    }

    VkWriteDescriptorSet WriteDescriptorImage(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo *imageInfo, uint32_t binding)
    {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;
        write.dstBinding = binding;
        write.dstSet = dstSet;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = imageInfo;
        return write;
    }

    void ResourceSetVk::UpdateTexture(Ref<Texture> texture, uint32_t binding)
    {
        Ref<TextureVk> textureVk = std::dynamic_pointer_cast<TextureVk>(texture);

        VkSamplerCreateInfo samplerInfo;
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = nullptr;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        samplerInfo.flags = 0;

        VkSampler sampler;
        if (vkCreateSampler(m_Device->GetVkDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create sampler");
        }

        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = sampler;
        imageBufferInfo.imageView = textureVk->GetImageView();
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writtenTexture = WriteDescriptorImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                   m_SamplerDescriptorSet[m_Device->GetCurrentFrameIndex()],
                                                                   &imageBufferInfo, binding);

        vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &writtenTexture, 0, nullptr);
    }

    VkDescriptorSetLayout ResourceSetVk::GetSamplerDescriptorSetLayout()
    {
        return m_SamplerLayout[m_Device->GetCurrentFrameIndex()];
    }

    VkDescriptorSet ResourceSetVk::GetSamplerDescriptorSet()
    {
        return m_SamplerDescriptorSet[m_Device->GetCurrentFrameIndex()];
    }

    VkDescriptorSetLayout ResourceSetVk::GetUniformBufferDescriptorSetLayout()
    {
        return m_UniformBufferLayout[m_Device->GetCurrentFrameIndex()];
    }

    VkDescriptorSet ResourceSetVk::GetUniformBufferrDescriptorSet()
    {
        return m_UniformBufferDescriptorSet[m_Device->GetCurrentFrameIndex()];
    }
}

#endif