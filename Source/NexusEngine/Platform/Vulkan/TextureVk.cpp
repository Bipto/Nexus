#if defined(NX_PLATFORM_VULKAN)

#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    TextureVk::TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec)
        : Texture(spec), m_GraphicsDevice(graphicsDevice)
    {
        VkDeviceSize imageSize = spec.Width * spec.Height * spec.NumberOfChannels;
        m_Format = GetVkTextureFormatFromNexusFormat(spec.Format);

        m_StagingBuffer = graphicsDevice->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        VkExtent3D imageExtent;
        imageExtent.width = spec.Width;
        imageExtent.height = spec.Height;
        imageExtent.depth = 1;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = nullptr;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = m_Format;
        imageInfo.extent = imageExtent;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo imageAllocInfo = {};
        imageAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if (vmaCreateImage(graphicsDevice->GetAllocator(), &imageInfo, &imageAllocInfo, &m_Image, &m_Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.image = m_Image;
        createInfo.format = m_Format;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        if (vkCreateImageView(graphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0f;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(graphicsDevice->GetPhysicalDevice(), &properties);

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        if (vkCreateSampler(graphicsDevice->GetVkDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture sampler");
        }
    }

    TextureVk::~TextureVk()
    {
        vkDestroySampler(m_GraphicsDevice->GetVkDevice(), m_Sampler, nullptr);
        vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_ImageView, nullptr);
        vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
    }

    ResourceHandle TextureVk::GetHandle()
    {
        return (ResourceHandle)m_ImageView;
    }

    void TextureVk::SetData(void *data, uint32_t size)
    {
        VkDeviceSize imageSize = m_Width * m_Height * m_NumOfChannels;

        void *buffer;
        vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
        memcpy(buffer, data, imageSize);
        vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

        VkExtent3D imageExtent;
        imageExtent.width = m_Width;
        imageExtent.height = m_Height;
        imageExtent.depth = 1;

        // upload texture to GPU
        {
            m_GraphicsDevice->ImmediateSubmit([&](VkCommandBuffer cmd)
                                              {
                                                VkImageSubresourceRange range;
                                                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                range.baseMipLevel = 0;
                                                range.levelCount = 1;
                                                range.baseArrayLayer = 0;
                                                range.layerCount = 1;

                                                VkImageMemoryBarrier imageBarrierToTransfer = {};
                                                imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                                                imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                                imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                                                imageBarrierToTransfer.image = m_Image;
                                                imageBarrierToTransfer.subresourceRange = range;
                                                imageBarrierToTransfer.srcAccessMask = 0;
                                                imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                                                vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToTransfer);

                                                VkBufferImageCopy copyRegion = {};
                                                copyRegion.bufferOffset = 0;
                                                copyRegion.bufferRowLength = 0;
                                                copyRegion.bufferImageHeight = 0;
                                                copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                copyRegion.imageSubresource.mipLevel = 0;
                                                copyRegion.imageSubresource.baseArrayLayer = 0;
                                                copyRegion.imageSubresource.layerCount = 1;
                                                copyRegion.imageExtent = imageExtent;
                                                vkCmdCopyBufferToImage(cmd, m_StagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

                                                VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
                                                imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                                                imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                                imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                                                imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                                                vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToReadable); });
        }
    }

    VkImage TextureVk::GetImage()
    {
        return m_Image;
    }

    VkImageView TextureVk::GetImageView()
    {
        return m_ImageView;
    }

    VkSampler TextureVk::GetSampler()
    {
        return m_Sampler;
    }
}

#endif