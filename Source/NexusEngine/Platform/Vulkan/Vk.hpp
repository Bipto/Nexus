#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "Nexus/Graphics/Framebuffer.hpp"
#include "Nexus/Graphics/ShaderDataType.hpp"

const uint32_t FRAMES_IN_FLIGHT = 3;

VkFormat GetVkTextureFormatFromNexusFormat(Nexus::Graphics::TextureFormat format);
VkFormat GetVkDepthFormatFromNexusFormat(Nexus::Graphics::DepthFormat format);
VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type);

struct AllocatedBuffer
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
};

#endif