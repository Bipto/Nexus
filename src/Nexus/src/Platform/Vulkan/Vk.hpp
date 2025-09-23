#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "vk_mem_alloc.h"
	#include "vulkan/vulkan.h"

	#include "Nexus-Core/Graphics/AccelerationStructure.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "Nexus-Core/Graphics/CommandQueue.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"
	#include "Nexus-Core/Graphics/SamplerState.hpp"
	#include "Nexus-Core/Graphics/ShaderDataType.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"

	#include "PNextBuilder.hpp"

const uint32_t FRAMES_IN_FLIGHT = 3;

namespace Nexus::Graphics
{
	class GraphicsDeviceVk;
	class ShaderModuleVk;
	class ResourceSetVk;
}	 // namespace Nexus::Graphics

namespace Nexus::Vk
{

	VkFormat					 GetVkPixelDataFormat(Nexus::Graphics::PixelFormat format);
	Nexus::Graphics::PixelFormat GetNxPixelFormatFromVkPixelFormat(VkFormat format);
	VkFormat					 GetShaderDataType(Nexus::Graphics::ShaderDataType type);

	void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode);
	VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

	VkCompareOp			  GetCompareOp(Nexus::Graphics::ComparisonFunction function);
	VkStencilOp			  GetStencilOp(Nexus::Graphics::StencilOperation operation);
	VkBlendOp			  GetVkBlendOp(Nexus::Graphics::BlendEquation function);
	VkBlendFactor		  GetVkBlendFactor(Nexus::Graphics::BlendFactor function);
	VkBorderColor		  GetVkBorderColor(Nexus::Graphics::BorderColor color);
	VkImageUsageFlagBits  GetVkImageUsageFlags(Graphics::PixelFormat format, uint8_t usage);
	VkImageCreateFlagBits GetVkImageCreateFlagBits(Graphics::TextureType textureType, uint8_t usage);
	VkImageType			  GetVkImageType(Graphics::TextureType textureType);
	VkImageViewType		  GetVkImageViewType(const Graphics::TextureDescription &spec);

	VkShaderStageFlagBits GetVkShaderStageFlags(Nexus::Graphics::ShaderStage stage);
	VkShaderStageFlagBits GetVkShaderStageFlagsFromShaderStages(const Nexus::Graphics::ShaderStageFlags &flags);

	VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexFormat format);
	VkFrontFace GetFrontFace(Nexus::Graphics::FrontFace frontFace);

	VkBufferCreateInfo		GetVkBufferCreateInfo(const Graphics::DeviceBufferDescription &desc, Graphics::GraphicsDeviceVk *device);
	VmaAllocationCreateInfo GetVmaAllocationCreateInfo(const Graphics::DeviceBufferDescription &desc, Graphics::GraphicsDeviceVk *device);

	bool				  SetObjectName(VkDevice device, VkObjectType type, uint64_t objectHandle, const char *name);
	uint32_t			  GetSampleCountFromVkSampleCountFlags(VkSampleCountFlags sampleCount);
	VkSampleCountFlagBits GetVkSampleCountFlagsFromSampleCount(uint32_t samples);

	VkImageAspectFlagBits GetAspectFlags(bool isDepth);

	VkAccelerationStructureTypeKHR		   GetAccelerationStructureType(Graphics::AccelerationStructureType type);
	VkBuildAccelerationStructureFlagsKHR   GetAccelerationStructureFlags(uint8_t flags);
	VkBuildAccelerationStructureModeKHR	   GetAccelerationStructureBuildMode(Graphics::AccelerationStructureBuildMode mode);
	VkGeometryTypeKHR					   GetAccelerationStructureGeometryType(Graphics::GeometryType type);
	VkGeometryFlagsKHR					   GetAccelerationStructureGeometryFlags(uint8_t flags);
	VkAccelerationStructureGeometryDataKHR GetAccelerationStructureGeometryData(const Graphics::AccelerationStructureGeometryDescription &geometry);
	VkDeviceOrHostAddressKHR			   GetDeviceOrHostAddress(Graphics::DeviceBufferAddress address);
	VkDeviceOrHostAddressConstKHR		   GetDeviceOrHostAddressConst(Graphics::DeviceBufferAddress address);
	VkFormat							   GetVulkanVertexFormat(Graphics::VertexFormat format);
	VkPresentModeKHR					   GetVulkanPresentMode(Graphics::PresentMode presentMode);

	std::vector<VkAccelerationStructureGeometryKHR> GetVulkanAccelerationStructureGeometries(
		const Graphics::AccelerationStructureGeometryBuildDescription &description);
	VkAccelerationStructureBuildGeometryInfoKHR GetGeometryBuildInfo(const Graphics::AccelerationStructureGeometryBuildDescription &description,
																	 const std::vector<VkAccelerationStructureGeometryKHR>		   &geometry);
	VkAccelerationStructureBuildRangeInfoKHR	GetAccelerationStructureBuildRange(Graphics::AccelerationStructureBuildRange range);

	struct VulkanRenderPassDescription
	{
		std::vector<VkFormat>	ColourAttachments = {};
		std::optional<VkFormat> DepthFormat		  = {};
		std::optional<VkFormat> ResolveFormat	  = {};
		VkSampleCountFlagBits	Samples			  = VK_SAMPLE_COUNT_1_BIT;
		bool					IsSwapchain		  = false;
	};

	VkRenderPass CreateRenderPass(Graphics::GraphicsDeviceVk *device, const VulkanRenderPassDescription &desc);

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

	// pipeline methods
	VkPipelineShaderStageCreateInfo					 CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
	VkPipelineInputAssemblyStateCreateInfo			 CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology);
	VkPipelineRasterizationStateCreateInfo			 CreateRasterizationStateCreateInfo(const Graphics::RasterizerStateDescription &rasterizerDesc);
	VkPipelineMultisampleStateCreateInfo			 CreateMultisampleStateCreateInfo(uint32_t sampleCount);
	std::vector<VkPipelineColorBlendAttachmentState> CreateColorBlendAttachmentStates(
		uint32_t											  colourAttachmentCount,
		const std::array<Graphics::BlendStateDescription, 8> &blendStates);
	VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateCreateInfo(const Graphics::DepthStencilDescription &depthStencilDesc);

	VkPrimitiveTopology GetPrimitiveTopology(Graphics::Topology topology);
	VkPolygonMode		GetPolygonMode(Graphics::FillMode fillMode);
	VkCullModeFlags		GetCullMode(Graphics::CullMode cullMode);

	void CreateVertexInputLayout(const std::vector<Graphics::VertexBufferLayout> &layouts,
								 std::vector<VkVertexInputAttributeDescription>	 &attributeDescriptions,
								 std::vector<VkVertexInputBindingDescription>	 &inputBindingDescriptions);

	VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(Nexus::Ref<Nexus::Graphics::ShaderModuleVk> module);

	VkPipelineLayout CreatePipelineLayout(Graphics::Pipeline				   *pipeline,
										  Graphics::GraphicsDeviceVk		   *device,
										  std::vector<VkDescriptorSetLayout>   &descriptorSetLayouts,
										  std::map<VkDescriptorType, uint32_t> &descriptorCounts);

	VkPipeline CreateGraphicsPipeline(VkRenderPass											  renderPass,
									  Graphics::GraphicsDeviceVk							 *device,
									  const Graphics::DepthStencilDescription				 &depthStencilDesc,
									  const Graphics::RasterizerStateDescription			 &rasterizerDesc,
									  uint32_t												  samples,
									  const std::vector<VkPipelineShaderStageCreateInfo>	 &shaderStages,
									  uint32_t												  colourTargetCount,
									  const std::array<Graphics::PixelFormat, 8>			 &colourFormats,
									  const std::array<Graphics::BlendStateDescription, 8>	 &blendStates,
									  Graphics::PixelFormat									  depthFormat,
									  VkPipelineLayout										  pipelineLayout,
									  Graphics::Topology									  topology,
									  const std::vector<Nexus::Graphics::VertexBufferLayout> &layouts);

	VkResult AcquireNextImage(Graphics::GraphicsDeviceVk *device,
							  VkSwapchainKHR			  swapchain,
							  uint64_t					  timeout,
							  VkSemaphore				  semaphore,
							  VkFence					  fence,
							  uint32_t					 *imageIndex);

	VkResult SubmitQueue(Graphics::GraphicsDeviceVk		   *device,
						 VkQueue							queue,
						 const std::vector<VkCommandBuffer> commandBuffers,
						 VkPipelineStageFlags				waitStageMask,
						 VkFence							fence);

	Graphics::QueueCapabilities GetNxQueueCapabilitiesFromVkQueuePropertyFlags(VkQueueFlags flags);

	VkQueue GetDeviceQueue(Graphics::GraphicsDeviceVk *device, const Graphics::CommandQueueDescription &description);

	VkAccessFlagBits		 GetAccessFlags(Graphics::GraphicsDeviceVk *device, Graphics::BarrierAccess access);
	VkPipelineStageFlagBits	 GetPipelineStageFlags(Graphics::GraphicsDeviceVk *device, Graphics::BarrierPipelineStage stage);
	VkAccessFlagBits2		 GetAccessFlags2(Graphics::GraphicsDeviceVk *device, Graphics::BarrierAccess access);
	VkPipelineStageFlagBits2 GetPipelineStageFlags2(Graphics::GraphicsDeviceVk *device, Graphics::BarrierPipelineStage stage);
	VkImageLayout			 GetImageLayout(Graphics::GraphicsDeviceVk *device, Graphics::TextureLayout layout);

}	 // namespace Nexus::Vk

#endif