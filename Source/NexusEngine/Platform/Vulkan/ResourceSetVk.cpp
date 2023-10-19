#if defined(NX_PLATFORM_VULKAN)

#include "ResourceSetVk.hpp"
#include "TextureVk.hpp"
#include "BufferVk.hpp"

namespace Nexus::Graphics
{
    ResourceSetVk::ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device)
        : ResourceSet(spec), m_Device(device)
    {
        // allocating uniform buffer descriptor set
        if (spec.UniformResourceBindings.size() > 0)
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
        if (spec.TextureBindings.size() > 0)
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
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

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
        if (spec.UniformResourceBindings.size() > 0)
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
        if (spec.TextureBindings.size() > 0)
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

    ResourceSetVk::~ResourceSetVk()
    {
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            if (HasTextures())
                vkDestroyDescriptorSetLayout(m_Device->GetVkDevice(), m_SamplerLayout[i], nullptr);

            if (HasUniformBuffers())
                vkDestroyDescriptorSetLayout(m_Device->GetVkDevice(), m_UniformBufferLayout[i], nullptr);
        }
        vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
    }

    void ResourceSetVk::WriteTexture(Texture *texture, uint32_t binding)
    {
        if (!HasTextures())
            return;

        TextureVk *textureVk = (TextureVk *)texture;

        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = textureVk->GetSampler();
        imageBufferInfo.imageView = textureVk->GetImageView();
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet textureToWrite = {};
        textureToWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureToWrite.pNext = nullptr;
        textureToWrite.dstBinding = binding;
        textureToWrite.dstSet = m_SamplerDescriptorSet[m_Device->GetCurrentFrameIndex()];
        textureToWrite.descriptorCount = 1;
        textureToWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureToWrite.pImageInfo = &imageBufferInfo;

        vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &textureToWrite, 0, nullptr);
    }

    void ResourceSetVk::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding)
    {
        if (!HasUniformBuffers())
            return;

        auto uniformBufferVk = (UniformBufferVk *)uniformBuffer;

        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = uniformBufferVk->GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = uniformBuffer->GetDescription().Size;

        VkWriteDescriptorSet uniformBufferToWrite = {};
        uniformBufferToWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferToWrite.pNext = nullptr;
        uniformBufferToWrite.dstBinding = binding;
        uniformBufferToWrite.descriptorCount = 1;
        uniformBufferToWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferToWrite.pBufferInfo = &bufferInfo;
        uniformBufferToWrite.dstSet = m_UniformBufferDescriptorSet[m_Device->GetCurrentFrameIndex()];

        vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);
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