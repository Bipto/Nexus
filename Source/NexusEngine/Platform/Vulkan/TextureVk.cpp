#if defined(NX_PLATFORM_VULKAN)

#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    TextureVk::TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec)
        : Texture(spec)
    {
        VkDeviceSize imageSize = spec.Width * spec.Height * spec.NumberOfChannels;
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        AllocatedBuffer stagingBuffer = graphicsDevice->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        void *data;
        vmaMapMemory(graphicsDevice->GetAllocator(), stagingBuffer.Allocation, &data);
        memcpy(data, spec.Data, imageSize);
        vmaUnmapMemory(graphicsDevice->GetAllocator(), stagingBuffer.Allocation);

        VkExtent3D imageExtent;
        imageExtent.width = spec.Width;
        imageExtent.height = spec.Height;
        imageExtent.depth = 1;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = nullptr;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = imageExtent;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo imageAllocInfo = {};
        imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (vmaCreateImage(graphicsDevice->GetAllocator(), &imageInfo, &imageAllocInfo, &m_Image, &m_Allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        // upload texture to GPU
        {
            graphicsDevice->ImmediateSubmit([&](VkCommandBuffer cmd)
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
                                                vkCmdCopyBufferToImage(cmd, stagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

                                                VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
                                                imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                                                imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                                imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                                                imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                                                vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToReadable); });
        }
    }

    TextureVk::~TextureVk()
    {
    }

    void *TextureVk::GetHandle()
    {
        return nullptr;
    }
}

#endif