#if defined(NX_PLATFORM_VULKAN)

#include "ResourceSetVk.hpp"
#include "TextureVk.hpp"
#include "BufferVk.hpp"

#include <algorithm>

namespace Nexus::Graphics
{
    ResourceSetVk::ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device)
        : ResourceSet(spec), m_Device(device)
    {
        std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> sets;
        std::map<VkDescriptorType, uint32_t> descriptorCounts;

        // create texture bindings
        for (const auto &textureInfo : spec.TextureBindings)
        {
            VkDescriptorSetLayoutBinding samplerBinding = {};
            samplerBinding.binding = textureInfo.Slot;
            samplerBinding.descriptorCount = 1;
            samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

            // if set does not exist, create it
            if (sets.find(textureInfo.Set) == sets.end())
            {
                sets[textureInfo.Set] = {};
            }

            if (descriptorCounts.find(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) == descriptorCounts.end())
            {
                descriptorCounts[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] = 0;
            }

            auto &set = sets[textureInfo.Set];
            set.push_back(samplerBinding);
            descriptorCounts[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER]++;
        }

        // create uniform buffer bindings
        for (const auto &uniformInfo : spec.UniformResourceBindings)
        {
            VkDescriptorSetLayoutBinding uniformBinding = {};
            uniformBinding.binding = uniformInfo.Binding;
            uniformBinding.descriptorCount = 1;
            uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

            // if set does not exist, create it
            if (sets.find(uniformInfo.Set) == sets.end())
            {
                sets[uniformInfo.Set] = {};
            }

            if (descriptorCounts.find(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) == descriptorCounts.end())
            {
                descriptorCounts[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] = 0;
            }

            auto &set = sets[uniformInfo.Set];
            set.push_back(uniformBinding);
            descriptorCounts[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]++;
        }

        // create descriptor set layouts
        for (const auto &set : sets)
        {
            VkDescriptorSetLayoutCreateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.pNext = nullptr;
            setInfo.flags = 0;
            setInfo.bindingCount = set.second.size();
            setInfo.pBindings = set.second.data();

            VkDescriptorSetLayout layout;
            if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &layout) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor set layout");
            }

            m_DescriptorSetLayouts[set.first] = layout;
        }

        // calculate required descriptor pool size
        std::vector<VkDescriptorPoolSize> sizes;
        for (const auto &descriptorCount : descriptorCounts)
        {
            VkDescriptorPoolSize size;
            size.type = descriptorCount.first;
            size.descriptorCount = descriptorCount.second;
            sizes.push_back(size);
        }

        // allocate descriptor pool
        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = FRAMES_IN_FLIGHT * sets.size();
        poolInfo.poolSizeCount = (uint32_t)sizes.size();
        poolInfo.pPoolSizes = sizes.data();

        if (vkCreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool");
        }

        // allocate descriptor sets
        m_DescriptorSets.resize(FRAMES_IN_FLIGHT);
        for (int frameIndex = 0; frameIndex < m_DescriptorSets.size(); frameIndex++)
        {
            auto &descriptorSets = m_DescriptorSets[frameIndex];

            for (int setIndex = 0; setIndex < m_DescriptorSetLayouts.size(); setIndex++)
            {
                VkDescriptorSetAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.pNext = nullptr;
                allocInfo.descriptorPool = m_DescriptorPool;
                allocInfo.descriptorSetCount = 1;
                allocInfo.pSetLayouts = &m_DescriptorSetLayouts[setIndex];

                if (vkAllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, &descriptorSets[setIndex]) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create descriptor set");
                }
            }
        }
    }

    ResourceSetVk::~ResourceSetVk()
    {
        for (const auto &layout : m_DescriptorSetLayouts)
        {
            vkDestroyDescriptorSetLayout(m_Device->GetVkDevice(), layout.second, nullptr);
        }

        vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
    }

    void ResourceSetVk::WriteTexture(Texture *texture, uint32_t set, uint32_t binding)
    {
        TextureVk *textureVk = (TextureVk *)texture;
        const auto &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

        VkDescriptorImageInfo imageBufferInfo = {};
        imageBufferInfo.sampler = textureVk->GetSampler();
        imageBufferInfo.imageView = textureVk->GetImageView();
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet textureToWrite = {};
        textureToWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureToWrite.pNext = nullptr;
        textureToWrite.dstBinding = binding;
        textureToWrite.dstSet = descriptorSets.at(set);
        textureToWrite.descriptorCount = 1;
        textureToWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureToWrite.pImageInfo = &imageBufferInfo;

        vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &textureToWrite, 0, nullptr);
    }

    void ResourceSetVk::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        UniformBufferVk *uniformBufferVk = (UniformBufferVk *)uniformBuffer;
        const auto &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

        VkDescriptorBufferInfo bufferInfo = {};
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
        uniformBufferToWrite.dstSet = descriptorSets.at(set);

        vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);
    }

    const std::map<uint32_t, VkDescriptorSetLayout> &ResourceSetVk::GetDescriptorSetLayouts() const
    {
        return m_DescriptorSetLayouts;
    }

    const std::vector<std::map<uint32_t, VkDescriptorSet>> &ResourceSetVk::GetDescriptorSets() const
    {
        return m_DescriptorSets;
    }
}

#endif