#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "Nexus/Graphics/Framebuffer.hpp"
#include "Nexus/Graphics/ShaderDataType.hpp"
#include "Nexus/Graphics/Multisample.hpp"

#include "Nexus/Graphics/SamplerState.hpp"

const uint32_t FRAMES_IN_FLIGHT = 3;

VkFormat GetVkTextureFormatFromNexusFormat(Nexus::Graphics::TextureFormat format);
VkFormat GetVkDepthFormatFromNexusFormat(Nexus::Graphics::DepthFormat format);
VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type);
VkSampleCountFlagBits GetVkSampleCount(Nexus::Graphics::MultiSamples samples);

void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode);
VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

VkCompareOp GetCompareOp(Nexus::Graphics::ComparisonFunction function);
VkBlendOp GetVkBlendOp(Nexus::Graphics::BlendEquation function);
VkBlendFactor GetVkBlendFactor(Nexus::Graphics::BlendFunction function);
VkBorderColor GetVkBorderColor(Nexus::Graphics::BorderColor color);

struct AllocatedBuffer
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
};

#endif