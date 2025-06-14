#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class ResourceSetVk : public ResourceSet
	{
	  public:
		ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device);
		~ResourceSetVk();

		virtual void WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name) override;
		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name) override;
		virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name) override;

		const std::map<uint32_t, VkDescriptorSetLayout>		   &GetDescriptorSetLayouts() const;
		const std::vector<std::map<uint32_t, VkDescriptorSet>> &GetDescriptorSets() const;

	  private:
		VkDescriptorPool m_DescriptorPool;

		std::map<uint32_t, VkDescriptorSetLayout>		 m_DescriptorSetLayouts;
		std::vector<std::map<uint32_t, VkDescriptorSet>> m_DescriptorSets;

		GraphicsDeviceVk *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif