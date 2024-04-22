#if defined(NX_PLATFORM_VULKAN)

#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    TextureVk::TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec)
        : Texture(spec), m_GraphicsDevice(graphicsDevice)
    {
        uint32_t numChannels = GetPixelFormatNumberOfChannels(m_Specification.Format);

        bool isDepth;
        VkDeviceSize imageSize = spec.Width * spec.Height * numChannels;
        VkImageUsageFlagBits usage = GetVkImageUsageFlags(spec.Usage, isDepth);
        m_Format = GetVkPixelDataFormat(spec.Format, isDepth);

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
        imageInfo.mipLevels = m_Specification.Levels;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;

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
        createInfo.subresourceRange.levelCount = m_Specification.Levels;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        if (vkCreateImageView(graphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }
    }

    TextureVk::~TextureVk()
    {
        vkDestroyImageView(m_GraphicsDevice->GetVkDevice(), m_ImageView, nullptr);
        vmaDestroyImage(m_GraphicsDevice->GetAllocator(), m_Image, m_Allocation);
    }

    void TextureVk::SetData(const void *data, uint32_t size, uint32_t level)
    {
        uint32_t numChannels = GetPixelFormatNumberOfChannels(m_Specification.Format);
        VkDeviceSize imageSize = size;

        void *buffer;
        vmaMapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation, &buffer);
        memcpy(buffer, data, imageSize);
        vmaUnmapMemory(m_GraphicsDevice->GetAllocator(), m_StagingBuffer.Allocation);

        VkExtent3D imageExtent;
        imageExtent.width = m_Specification.Width;
        imageExtent.height = m_Specification.Height;
        imageExtent.depth = 1;

        // upload texture to GPU
        {
            m_GraphicsDevice->ImmediateSubmit([&](VkCommandBuffer cmd)
                                              {
                                                VkImageSubresourceRange range;
                                                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                                                range.baseMipLevel = level;
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

    std::vector<std::byte> TextureVk::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        return std::vector<std::byte>();
    }

    VkImage TextureVk::GetImage()
    {
        return m_Image;
    }

    VkImageView TextureVk::GetImageView()
    {
        return m_ImageView;
    }

    VkImageLayout TextureVk::GetLayout()
    {
        return m_Layout;
    }

    void TextureVk::SetLayout(VkImageLayout layout)
    {
        m_Layout = layout;
    }
}

#endif