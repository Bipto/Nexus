#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "Nexus-Core/Graphics/SamplerState.hpp"
	#include "Nexus-Core/Graphics/ShaderDataType.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "vk_mem_alloc.h"
	#include "vulkan/vulkan.h"

const uint32_t FRAMES_IN_FLIGHT = 3;

namespace Nexus::Vk
{

	VkFormat					 GetVkPixelDataFormat(Nexus::Graphics::PixelFormat format, bool depthFormat);
	Nexus::Graphics::PixelFormat GetNxPixelFormatFromVkPixelFormat(VkFormat format);
	VkFormat					 GetShaderDataType(Nexus::Graphics::ShaderDataType type);

	void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode);
	VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

	VkCompareOp			  GetCompareOp(Nexus::Graphics::ComparisonFunction function);
	VkStencilOp			  GetStencilOp(Nexus::Graphics::StencilOperation operation);
	VkBlendOp			  GetVkBlendOp(Nexus::Graphics::BlendEquation function);
	VkBlendFactor		  GetVkBlendFactor(Nexus::Graphics::BlendFactor function);
	VkBorderColor		  GetVkBorderColor(Nexus::Graphics::BorderColor color);
	VkImageUsageFlagBits  GetVkImageUsageFlags(uint8_t usage);
	VkImageCreateFlagBits GetVkImageCreateFlagBits(uint8_t usage);
	VkImageType			  GetVkImageType(Graphics::TextureType textureType);
	VkImageViewType		  GetVkImageViewType(const Graphics::TextureSpecification &spec);

	VkShaderStageFlagBits GetVkShaderStageFlags(Nexus::Graphics::ShaderStage stage);

	VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format);
	VkFrontFace GetFrontFace(Nexus::Graphics::FrontFace frontFace);

	VkBufferCreateInfo		GetVkBufferCreateInfo(const Graphics::DeviceBufferDescription &desc);
	VmaAllocationCreateInfo GetVmaAllocationCreateInfo(const Graphics::DeviceBufferDescription &desc);

	bool				  SetObjectName(VkDevice device, VkObjectType type, uint64_t objectHandle, const char *name);
	uint32_t			  GetSampleCountFromVkSampleCountFlags(VkSampleCountFlags sampleCount);
	VkSampleCountFlagBits GetVkSampleCountFlagsFromSampleCount(uint32_t samples);

	VkImageAspectFlagBits GetAspectFlags(Graphics::ImageAspect aspect);

	struct VulkanRenderPassDescription
	{
		std::vector<VkFormat>	ColourAttachments = {};
		std::optional<VkFormat> DepthFormat		  = {};
		std::optional<VkFormat> ResolveFormat	  = {};
		VkSampleCountFlagBits	Samples			  = VK_SAMPLE_COUNT_1_BIT;
		bool					IsSwapchain		  = false;
	};

	VkRenderPass CreateRenderPass(VkDevice device, const VulkanRenderPassDescription &desc);

	struct VulkanFramebufferDescription
	{
		std::vector<VkImageView> ColourImageViews = {};
		VkImageView				 DepthImageView	  = VK_NULL_HANDLE;
		VkImageView				 ResolveImageView = VK_NULL_HANDLE;
		VkRenderPass			 VulkanRenderPass = VK_NULL_HANDLE;
		uint32_t				 Width			  = 0;
		uint32_t				 Height			  = 0;
	};

	VkFramebuffer CreateFramebuffer(VkDevice device, const VulkanFramebufferDescription &desc);

	struct AllocatedBuffer
	{
		VkBuffer	  Buffer;
		VmaAllocation Allocation;
	};
}	 // namespace Nexus::Vk

#endif