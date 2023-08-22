#pragma once

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "Core/Graphics/Framebuffer.hpp"
#include "Core/Graphics/ShaderDataType.hpp"

VkFormat GetVkFormatFromNexusFormat(Nexus::Graphics::TextureFormat format);
VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type);

struct AllocatedBuffer
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
};