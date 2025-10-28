#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "Nexus-Core/Graphics/ShaderResources.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class ResourceSetVk final : public IResourceSet
	{
	  public:
		ResourceSetVk(Ref<Pipeline> pipeline, GraphicsDeviceVk *device);
		~ResourceSetVk();

		void WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name) final;
		void WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name) final;
		void WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name) final;
		void WriteStorageBuffer(const StorageBufferView &view, const std::string &name) final;
		void WriteDynamicStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name) final;
		void WriteStorageImage(const StorageImageView &view, const std::string &name) final;
		void WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name) final;
		void WriteSampledImage(Ref<ITextureView> textureView, const std::string &name) final;
		void WriteSampler(Ref<ISampler> sampler, const std::string &name) final;
		void WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructure, const std::string &name) final;
		void WriteTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name) final;
		void Flush() final;

		const std::map<uint32_t, VkDescriptorSet> &GetDescriptorSets() const;
		std::optional<VkShaderStageFlags>		   GetPushConstantsStageFlags(const std::string &name) const;
		VkPipelineLayout						   GetPipelineLayout();

	  private:
		VkDescriptorPool					m_DescriptorPool = VK_NULL_HANDLE;
		std::map<uint32_t, VkDescriptorSet> m_DescriptorSets = {};

		Ref<Pipeline> m_Pipeline = nullptr;

		GraphicsDeviceVk						 *m_Device			   = nullptr;
		std::map<std::string, VkShaderStageFlags> m_PushConstantRanges = {};
	};
}	 // namespace Nexus::Graphics

#endif