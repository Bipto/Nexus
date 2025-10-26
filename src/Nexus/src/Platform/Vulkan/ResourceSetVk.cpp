#if defined(NX_PLATFORM_VULKAN)

	#include "ResourceSetVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "PipelineVk.hpp"
	#include "SamplerVk.hpp"
	#include "TextureViewVk.hpp"
	#include "TextureVk.hpp"

namespace Nexus::Graphics
{
	ResourceSetVk::ResourceSetVk(Ref<Pipeline> pipeline, GraphicsDeviceVk *device) : IResourceSet(pipeline), m_Device(device)
	{
		const GladVulkanContext &context		= m_Device->GetVulkanContext();
		Ref<PipelineVk>			 vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(pipeline);

		// calculate required descriptor pool size
		std::vector<VkDescriptorPoolSize> sizes = {};
		for (const auto &[descriptorType, descriptorCount] : vulkanPipeline->GetDescriptorCounts())
		{
			VkDescriptorPoolSize size = {};
			size.type				  = descriptorType;
			size.descriptorCount	  = descriptorCount;
			sizes.push_back(size);
		}

		const std::map<uint32_t, VkDescriptorSetLayout> &descriptorSetLayouts = vulkanPipeline->GetDescriptorSetLayouts();

		// allocate descriptor pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType						= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags						= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets					= descriptorSetLayouts.size();
		poolInfo.poolSizeCount				= (uint32_t)sizes.size();
		poolInfo.pPoolSizes					= sizes.data();

		if (context.CreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool");
		}

		// allocate descriptor sets
		for (const auto &[setIndex, setLayout] : descriptorSetLayouts)
		{
			VkDescriptorSet &descriptorSet = m_DescriptorSets[setIndex];

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType						  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.pNext						  = nullptr;
			allocInfo.descriptorPool			  = m_DescriptorPool;
			allocInfo.descriptorSetCount		  = 1;
			allocInfo.pSetLayouts				  = &setLayout;

			if (context.AllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create descriptor set");
			}
		}

		m_PushConstantRanges = Vk::GetPushConstantRanges(pipeline.get(), device);
	}

	ResourceSetVk::~ResourceSetVk()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.DestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
	}

	void ResourceSetVk::WriteStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (Ref<IDeviceBuffer> buffer = storageBuffer.BufferHandle)
		{
			NX_VALIDATE(buffer->CheckUsage(Graphics::BufferUsage::Storage), "Attempting to bind a buffer that is not a storage buffer");

			Ref<DeviceBufferVk> storageBufferVk = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);

			ShaderResource &resourceInfo = m_ShaderResources.at(name);

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer				  = storageBufferVk->GetVkBuffer();
			bufferInfo.offset				  = storageBuffer.Offset;
			bufferInfo.range				  = storageBuffer.SizeInBytes;

			VkWriteDescriptorSet uniformBufferToWrite = {};
			uniformBufferToWrite.sType				  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uniformBufferToWrite.pNext				  = nullptr;
			uniformBufferToWrite.dstBinding			  = resourceInfo.Binding;
			uniformBufferToWrite.descriptorCount	  = 1;
			uniformBufferToWrite.descriptorType		  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			uniformBufferToWrite.pBufferInfo		  = &bufferInfo;
			uniformBufferToWrite.dstSet				  = m_DescriptorSets.at(resourceInfo.Set);

			context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);

			m_BoundStorageBuffers[name] = storageBuffer;
		}
	}

	void ResourceSetVk::WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (Ref<IDeviceBuffer> buffer = uniformBuffer.BufferHandle)
		{
			NX_VALIDATE(buffer->CheckUsage(Graphics::BufferUsage::Uniform), "Attempting to bind a buffer that is not a uniform buffer");

			Ref<DeviceBufferVk> uniformBufferVk = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			const auto		   &descriptorSets	= m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

			ShaderResource &resourceInfo = m_ShaderResources.at(name);

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer				  = uniformBufferVk->GetVkBuffer();
			bufferInfo.offset				  = uniformBuffer.Offset;
			bufferInfo.range				  = uniformBuffer.Size;

			VkWriteDescriptorSet uniformBufferToWrite = {};
			uniformBufferToWrite.sType				  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uniformBufferToWrite.pNext				  = nullptr;
			uniformBufferToWrite.dstBinding			  = resourceInfo.Binding;
			uniformBufferToWrite.descriptorCount	  = 1;
			uniformBufferToWrite.descriptorType		  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uniformBufferToWrite.pBufferInfo		  = &bufferInfo;
			uniformBufferToWrite.dstSet				  = m_DescriptorSets.at(resourceInfo.Set);

			context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);

			m_BoundUniformBuffers[name] = uniformBuffer;
		}
	}

	void ResourceSetVk::WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name)
	{
		Ref<TextureViewVk> textureViewVk = std::dynamic_pointer_cast<TextureViewVk>(combinedImageSampler.ImageTexture);
		Ref<SamplerVk>	   samplerVk	 = std::dynamic_pointer_cast<SamplerVk>(combinedImageSampler.ImageSampler);

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		ShaderResource &resourceInfo = m_ShaderResources.at(name);
		VkImageLayout	layout		 = Vk::GetImageLayout(m_Device, TextureLayout::ShaderReadOnlyOptimal);

		VkDescriptorImageInfo imageBufferInfo = {};
		imageBufferInfo.imageView			  = textureViewVk->GetVkImageView();
		imageBufferInfo.sampler				  = samplerVk->GetSampler();
		imageBufferInfo.imageLayout			  = layout;

		VkWriteDescriptorSet textureToWrite = {};
		textureToWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureToWrite.pNext				= nullptr;
		textureToWrite.dstBinding			= resourceInfo.Binding;
		textureToWrite.dstSet				= m_DescriptorSets.at(resourceInfo.Set);
		textureToWrite.descriptorCount		= 1;
		textureToWrite.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureToWrite.pImageInfo			= &imageBufferInfo;

		context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &textureToWrite, 0, nullptr);

		m_BoundCombinedImageSamplers[name] = combinedImageSampler;
	}

	void ResourceSetVk::WriteStorageImage(const StorageImageView &view, const std::string &name)
	{
		Ref<TextureVk> textureVk = std::dynamic_pointer_cast<TextureVk>(view.TextureHandle);

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		ShaderResource &resourceInfo = m_ShaderResources.at(name);

		VkImageLayout layout = Vk::GetImageLayout(m_Device, view.Layout);

		VulkanTextureViewInfo viewInfo = {};
		viewInfo.BaseMipLevel		   = view.MipLevel;
		viewInfo.LevelCount			   = 1;
		viewInfo.BaseArrayLayer		   = view.ArrayLayer;
		viewInfo.LayerCount			   = 1;

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageView				= textureVk->GetImageView(viewInfo);
		imageInfo.imageLayout			= VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstBinding			= resourceInfo.Binding;
		writeDescriptorSet.dstSet				= m_DescriptorSets.at(resourceInfo.Set);
		writeDescriptorSet.descriptorCount		= 1;
		writeDescriptorSet.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.pImageInfo			= &imageInfo;

		context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &writeDescriptorSet, 0, nullptr);

		m_BoundStorageImages[name] = view;
	}

	const std::map<uint32_t, VkDescriptorSet> &ResourceSetVk::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}

	std::optional<VkShaderStageFlags> ResourceSetVk::GetPushConstantsStageFlags(const std::string &name) const
	{
		if (m_PushConstantRanges.contains(name))
		{
			return m_PushConstantRanges.at(name);
		}

		return std::nullopt;
	}
}	 // namespace Nexus::Graphics

#endif