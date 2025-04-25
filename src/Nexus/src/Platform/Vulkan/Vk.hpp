#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "Nexus-Core/Graphics/SamplerState.hpp"
	#include "Nexus-Core/Graphics/ShaderDataType.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "vk_mem_alloc.h"
	#include "vulkan/vulkan.h"

const uint32_t FRAMES_IN_FLIGHT = 3;

namespace Nexus::Vk
{

	VkFormat			  GetVkPixelDataFormat(Nexus::Graphics::PixelFormat format, bool depthFormat);
	Nexus::Graphics::PixelFormat GetNxPixelFormatFromVkPixelFormat(VkFormat format);
	VkFormat					 GetShaderDataType(Nexus::Graphics::ShaderDataType type);

	void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode);
	VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

	VkCompareOp			 GetCompareOp(Nexus::Graphics::ComparisonFunction function);
	VkBlendOp			 GetVkBlendOp(Nexus::Graphics::BlendEquation function);
	VkBlendFactor		 GetVkBlendFactor(Nexus::Graphics::BlendFactor function);
	VkBorderColor		 GetVkBorderColor(Nexus::Graphics::BorderColor color);
	VkImageUsageFlagBits GetVkImageUsageFlags(const std::vector<Nexus::Graphics::TextureUsage> &usage, bool &isDepth);

	VkShaderStageFlagBits GetVkShaderStageFlags(Nexus::Graphics::ShaderStage stage);

	VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format);
	VkFrontFace GetFrontFace(Nexus::Graphics::FrontFace frontFace);

	VkBufferCreateInfo		GetVkBufferCreateInfo(const Graphics::DeviceBufferDescription &desc);
	VmaAllocationCreateInfo GetVmaAllocationCreateInfo(const Graphics::DeviceBufferDescription &desc);

	bool SetObjectName(VkDevice device, VkObjectType type, uint64_t objectHandle, const char *name);
	uint32_t			  GetSampleCountFromVkSampleCountFlags(VkSampleCountFlags sampleCount);
	VkSampleCountFlagBits GetVkSampleCountFlagsFromSampleCount(uint32_t samples);

	struct AllocatedBuffer
	{
		VkBuffer	  Buffer;
		VmaAllocation Allocation;
	};
}	 // namespace Nexus::Vk

#endif