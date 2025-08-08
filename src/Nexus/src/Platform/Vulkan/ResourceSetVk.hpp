#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/ShaderResources.hpp"
	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class ResourceSetVk : public ResourceSet
	{
	  public:
		ResourceSetVk(Ref<Pipeline> pipeline, GraphicsDeviceVk *device);
		~ResourceSetVk();

		virtual void WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name) override;
		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name) override;
		virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name) override;

		const std::vector<std::map<uint32_t, VkDescriptorSet>> &GetDescriptorSets() const;

	  private:
		VkDescriptorPool								 m_DescriptorPool;
		std::vector<std::map<uint32_t, VkDescriptorSet>> m_DescriptorSets;

		GraphicsDeviceVk *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif