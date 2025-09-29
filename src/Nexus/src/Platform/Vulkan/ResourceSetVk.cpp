#if defined(NX_PLATFORM_VULKAN)

	#include "ResourceSetVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "PipelineVk.hpp"
	#include "SamplerVk.hpp"
	#include "TextureVk.hpp"

namespace Nexus::Graphics
{
	ResourceSetVk::ResourceSetVk(Ref<Pipeline> pipeline, GraphicsDeviceVk *device) : ResourceSet(pipeline), m_Device(device)
	{
		const GladVulkanContext &context		= m_Device->GetVulkanContext();
		Ref<PipelineVk>			 vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(pipeline);

		// calculate required descriptor pool size
		std::vector<VkDescriptorPoolSize> sizes;
		for (const auto &descriptorCount : vulkanPipeline->GetDescriptorCounts())
		{
			VkDescriptorPoolSize size;
			size.type			 = descriptorCount.first;
			size.descriptorCount = descriptorCount.second;
			sizes.push_back(size);
		}

		const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts = vulkanPipeline->GetDescriptorSetLayouts();

		// allocate descriptor pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType						= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags						= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets					= FRAMES_IN_FLIGHT * descriptorSetLayouts.size();
		poolInfo.poolSizeCount				= (uint32_t)sizes.size();
		poolInfo.pPoolSizes					= sizes.data();

		if (context.CreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool");
		}

		// allocate descriptor sets
		m_DescriptorSets.resize(FRAMES_IN_FLIGHT);
		for (int frameIndex = 0; frameIndex < m_DescriptorSets.size(); frameIndex++)
		{
			auto &descriptorSets = m_DescriptorSets[frameIndex];

			for (uint32_t setIndex = 0; setIndex < descriptorSetLayouts.size(); setIndex++)
			{
				VkDescriptorSetAllocateInfo allocInfo = {};
				allocInfo.sType						  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.pNext						  = nullptr;
				allocInfo.descriptorPool			  = m_DescriptorPool;
				allocInfo.descriptorSetCount		  = 1;
				allocInfo.pSetLayouts				  = &descriptorSetLayouts[setIndex];

				if (context.AllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, &descriptorSets[setIndex]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create descriptor set");
				}
			}
		}
	}

	ResourceSetVk::~ResourceSetVk()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.DestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
	}

	void ResourceSetVk::WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (Ref<DeviceBuffer> buffer = storageBuffer.BufferHandle)
		{
			NX_VALIDATE(buffer->CheckUsage(Graphics::BufferUsage::Storage), "Attempting to bind a buffer that is not a storage buffer");

			Ref<DeviceBufferVk> storageBufferVk = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			const auto		   &descriptorSets	= m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

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
			uniformBufferToWrite.dstSet				  = descriptorSets.at(resourceInfo.Set);

			context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);

			m_BoundStorageBuffers[name] = storageBuffer;
		}
	}

	void ResourceSetVk::WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (Ref<DeviceBuffer> buffer = uniformBuffer.BufferHandle)
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
			uniformBufferToWrite.dstSet				  = descriptorSets.at(resourceInfo.Set);

			context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);

			m_BoundUniformBuffers[name] = uniformBuffer;
		}
	}

	void ResourceSetVk::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
	{
		Ref<TextureVk> textureVk	  = std::dynamic_pointer_cast<TextureVk>(texture);
		Ref<SamplerVk> samplerVk	  = std::dynamic_pointer_cast<SamplerVk>(sampler);
		const auto	  &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		ShaderResource &resourceInfo = m_ShaderResources.at(name);
		VkImageLayout	layout		 = Vk::GetImageLayout(m_Device, TextureLayout::ShaderReadOnlyOptimal);

		VkDescriptorImageInfo imageBufferInfo = {};
		imageBufferInfo.imageView			  = textureVk->GetImageView();
		imageBufferInfo.sampler				  = samplerVk->GetSampler();
		imageBufferInfo.imageLayout			  = layout;

		VkWriteDescriptorSet textureToWrite = {};
		textureToWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureToWrite.pNext				= nullptr;
		textureToWrite.dstBinding			= resourceInfo.Binding;
		textureToWrite.dstSet				= descriptorSets.at(resourceInfo.Set);
		textureToWrite.descriptorCount		= 1;
		textureToWrite.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureToWrite.pImageInfo			= &imageBufferInfo;

		context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &textureToWrite, 0, nullptr);

		CombinedImageSampler ciSampler {};
		ciSampler.ImageTexture			   = texture;
		ciSampler.ImageSampler			   = sampler;
		m_BoundCombinedImageSamplers[name] = ciSampler;
	}

	void ResourceSetVk::WriteStorageImage(StorageImageView view, const std::string &name)
	{
		Ref<TextureVk> textureVk	  = std::dynamic_pointer_cast<TextureVk>(view.TextureHandle);
		const auto	  &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		ShaderResource &resourceInfo = m_ShaderResources.at(name);

		VkImageLayout layout = Vk::GetImageLayout(m_Device, view.Layout);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageView				= textureVk->GetImageView();
		imageInfo.imageLayout			= VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstBinding			= resourceInfo.Binding;
		writeDescriptorSet.dstSet				= descriptorSets.at(resourceInfo.Set);
		writeDescriptorSet.descriptorCount		= 1;
		writeDescriptorSet.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.pImageInfo			= &imageInfo;

		context.UpdateDescriptorSets(m_Device->GetVkDevice(), 1, &writeDescriptorSet, 0, nullptr);

		m_BoundStorageImages[name] = view;
	}

	const std::vector<std::map<uint32_t, VkDescriptorSet>> &ResourceSetVk::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}
}	 // namespace Nexus::Graphics

#endif