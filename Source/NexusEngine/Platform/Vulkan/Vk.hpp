#pragma once

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "Core/Graphics/Framebuffer.hpp"
#include "Core/Graphics/ShaderDataType.hpp"

const uint32_t FRAMES_IN_FLIGHT = 3;

VkFormat GetVkFormatFromNexusFormat(Nexus::Graphics::TextureFormat format);
VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type);

struct AllocatedBuffer
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
};