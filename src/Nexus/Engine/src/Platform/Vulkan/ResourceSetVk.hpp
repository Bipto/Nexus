#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "RHI/ResourceSet.hpp"
	#include "RHI/ShaderResources.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class ResourceSetVk final : public IResourceSet
	{
	  public:
		struct DynamicOffsetDescription
		{
			uint32_t Set	= 0;
			size_t	 Offset = 0;
		};

	  public:
		ResourceSetVk(Ref<IPipeline> pipeline, GraphicsDeviceVk *device);
		~ResourceSetVk();
		void Flush() final;

		void Bind(const GladVulkanContext							 &context,
				  VkCommandBuffer									  cmd,
				  PipelineVk										 *pipeline,
				  VkPipelineBindPoint								  bindPoint,
				  const std::map<std::string, std::vector<uint32_t>> &dynamicOffsets);

		const std::map<uint32_t, VkDescriptorSet> &GetDescriptorSets() const;
		std::optional<VkShaderStageFlags>		   GetPushConstantsStageFlags(const std::string &name) const;

	  private:
		VkDescriptorPool					m_DescriptorPool = VK_NULL_HANDLE;
		std::map<uint32_t, VkDescriptorSet> m_DescriptorSets = {};
		VkShaderStageFlags					m_PipelineStages = {};

		std::map<uint32_t, std::vector<uint32_t>>		m_DynamicOffsets   = {};
		std::map<std::string, DynamicOffsetDescription> m_DynamicOffsetMap = {};

		Ref<IPipeline> m_Pipeline = nullptr;

		GraphicsDeviceVk						 *m_Device			   = nullptr;
		std::map<std::string, VkShaderStageFlags> m_PushConstantRanges = {};
	};
}	 // namespace Nexus::Graphics

#endif