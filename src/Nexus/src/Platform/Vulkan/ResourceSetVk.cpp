#if defined(NX_PLATFORM_VULKAN)

	#include "ResourceSetVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "SamplerVk.hpp"
	#include "TextureVk.hpp"

namespace Nexus::Graphics
{
	ResourceSetVk::ResourceSetVk(const ResourceSetSpecification &spec, GraphicsDeviceVk *device) : ResourceSet(spec), m_Device(device)
	{
		std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> sets;
		std::map<VkDescriptorType, uint32_t>						  descriptorCounts;

		// create texture bindings
		for (const auto &textureBinding : spec.SampledImages)
		{
			VkDescriptorSetLayoutBinding descriptorBinding = {};
			descriptorBinding.binding					   = textureBinding.Binding;
			descriptorBinding.descriptorCount			   = 1;
			descriptorBinding.descriptorType			   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorBinding.stageFlags				   = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

			// if this set has not been created yet, then create it
			if (sets.find(textureBinding.Set) == sets.end())
			{
				sets[textureBinding.Set] = {};
			}

			// if we do not have a count for this descriptor type, create one
			if (descriptorCounts.find(descriptorBinding.descriptorType) == descriptorCounts.end())
			{
				descriptorCounts[descriptorBinding.descriptorType] = 0;
			}

			auto &set = sets[textureBinding.Set];
			set.push_back(descriptorBinding);
			descriptorCounts[descriptorBinding.descriptorType]++;
		}

		for (const auto &uniformBufferBinding : spec.UniformBuffers)
		{
			VkDescriptorSetLayoutBinding descriptorBinding = {};
			descriptorBinding.binding					   = uniformBufferBinding.Binding;
			descriptorBinding.descriptorCount			   = 1;
			descriptorBinding.descriptorType			   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorBinding.stageFlags				   = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

			// if this set has not been created yet, then create it
			if (sets.find(uniformBufferBinding.Set) == sets.end())
			{
				sets[uniformBufferBinding.Set] = {};
			}

			// if we do not have a count for this descriptor type, create one
			if (descriptorCounts.find(descriptorBinding.descriptorType) == descriptorCounts.end())
			{
				descriptorCounts[descriptorBinding.descriptorType] = 0;
			}

			auto &set = sets[uniformBufferBinding.Set];
			set.push_back(descriptorBinding);
			descriptorCounts[descriptorBinding.descriptorType]++;
		}

		for (const auto &storageImageBinding : spec.StorageImages)
		{
			VkDescriptorSetLayoutBinding descriptorBinding = {};
			descriptorBinding.binding					   = storageImageBinding.Binding;
			descriptorBinding.descriptorCount			   = 1;
			descriptorBinding.descriptorType			   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorBinding.stageFlags				   = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

			// if this set has not been created yet, then create it
			if (sets.find(storageImageBinding.Set) == sets.end())
			{
				sets[storageImageBinding.Set] = {};
			}

			// if we do not have a count for this descriptor type, create one
			if (descriptorCounts.find(descriptorBinding.descriptorType) == descriptorCounts.end())
			{
				descriptorCounts[descriptorBinding.descriptorType] = 0;
			}

			auto &set = sets[storageImageBinding.Set];
			set.push_back(descriptorBinding);
			descriptorCounts[descriptorBinding.descriptorType]++;
		}

		// create descriptor sets
		for (const auto &set : sets)
		{
			VkDescriptorSetLayoutCreateInfo setInfo = {};
			setInfo.sType							= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setInfo.pNext							= nullptr;
			setInfo.flags							= 0;
			setInfo.bindingCount					= set.second.size();
			setInfo.pBindings						= set.second.data();

			VkDescriptorSetLayout layout;
			if (vkCreateDescriptorSetLayout(device->GetVkDevice(), &setInfo, nullptr, &layout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create descriptor set layout");
			}

			m_DescriptorSetLayouts[set.first] = layout;
		}

		// calculate required descriptor pool size
		std::vector<VkDescriptorPoolSize> sizes;
		for (const auto &descriptorCount : descriptorCounts)
		{
			VkDescriptorPoolSize size;
			size.type			 = descriptorCount.first;
			size.descriptorCount = descriptorCount.second;
			sizes.push_back(size);
		}

		// allocate descriptor pool
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType						= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags						= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets					= FRAMES_IN_FLIGHT * sets.size();
		poolInfo.poolSizeCount				= (uint32_t)sizes.size();
		poolInfo.pPoolSizes					= sizes.data();

		if (vkCreateDescriptorPool(device->GetVkDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool");
		}

		// allocate descriptor sets
		m_DescriptorSets.resize(FRAMES_IN_FLIGHT);
		for (int frameIndex = 0; frameIndex < m_DescriptorSets.size(); frameIndex++)
		{
			auto &descriptorSets = m_DescriptorSets[frameIndex];

			for (int setIndex = 0; setIndex < m_DescriptorSetLayouts.size(); setIndex++)
			{
				VkDescriptorSetAllocateInfo allocInfo = {};
				allocInfo.sType						  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.pNext						  = nullptr;
				allocInfo.descriptorPool			  = m_DescriptorPool;
				allocInfo.descriptorSetCount		  = 1;
				allocInfo.pSetLayouts				  = &m_DescriptorSetLayouts[setIndex];

				if (vkAllocateDescriptorSets(m_Device->GetVkDevice(), &allocInfo, &descriptorSets[setIndex]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create descriptor set");
				}
			}
		}
	}

	ResourceSetVk::~ResourceSetVk()
	{
		for (const auto &layout : m_DescriptorSetLayouts) { vkDestroyDescriptorSetLayout(m_Device->GetVkDevice(), layout.second, nullptr); }

		vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
	}

	void ResourceSetVk::WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)
	{
		if (Ref<DeviceBuffer> buffer = uniformBuffer.BufferHandle)
		{
			NX_ASSERT(buffer->CheckUsage(Graphics::BufferUsage::Uniform), "Attempting to bind a buffer that is not a uniform buffer");

			Ref<DeviceBufferVk> uniformBufferVk = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			const auto		   &descriptorSets	= m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

			const BindingInfo &info = m_UniformBufferBindingInfos.at(name);

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer				  = uniformBufferVk->GetVkBuffer();
			bufferInfo.offset				  = uniformBuffer.Offset;
			bufferInfo.range				  = uniformBuffer.Size;

			VkWriteDescriptorSet uniformBufferToWrite = {};
			uniformBufferToWrite.sType				  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uniformBufferToWrite.pNext				  = nullptr;
			uniformBufferToWrite.dstBinding			  = info.Binding;
			uniformBufferToWrite.descriptorCount	  = 1;
			uniformBufferToWrite.descriptorType		  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uniformBufferToWrite.pBufferInfo		  = &bufferInfo;
			uniformBufferToWrite.dstSet				  = descriptorSets.at(info.Set);

			vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &uniformBufferToWrite, 0, nullptr);

			m_BoundUniformBuffers[name] = uniformBuffer;
		}
	}

	void ResourceSetVk::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
	{
		Ref<TextureVk>	  textureVk		 = std::dynamic_pointer_cast<TextureVk>(texture);
		Ref<SamplerVk>	  samplerVk		 = std::dynamic_pointer_cast<SamplerVk>(sampler);
		const auto		 &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

		const BindingInfo &info = m_CombinedImageSamplerBindingInfos.at(name);

		m_Device->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				for (uint32_t i = 0; i < textureVk->GetSpecification().MipLevels; i++)
				{
					m_Device->TransitionVulkanImageLayout(cmd,
														  textureVk->GetImage(),
														  i,
														  0,
														  textureVk->GetImageLayout(0, i),
														  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
														  VK_IMAGE_ASPECT_COLOR_BIT);
					textureVk->SetImageLayout(0, i, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				}
			});

		VkDescriptorImageInfo imageBufferInfo = {};
		imageBufferInfo.imageView			  = textureVk->GetImageView();
		imageBufferInfo.sampler				  = samplerVk->GetSampler();
		imageBufferInfo.imageLayout			  = textureVk->GetImageLayout(0, 0);

		VkWriteDescriptorSet textureToWrite = {};
		textureToWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureToWrite.pNext				= nullptr;
		textureToWrite.dstBinding			= info.Binding;
		textureToWrite.dstSet				= descriptorSets.at(info.Set);
		textureToWrite.descriptorCount		= 1;
		textureToWrite.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureToWrite.pImageInfo			= &imageBufferInfo;

		vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &textureToWrite, 0, nullptr);

		CombinedImageSampler ciSampler {};
		ciSampler.ImageTexture			   = texture;
		ciSampler.ImageSampler			   = sampler;
		m_BoundCombinedImageSamplers[name] = ciSampler;
	}

	void ResourceSetVk::WriteStorageImage(StorageImageView view, const std::string &name)
	{
		Ref<TextureVk> textureVk	  = std::dynamic_pointer_cast<TextureVk>(view.TextureHandle);
		const auto	 &descriptorSets = m_DescriptorSets[m_Device->GetCurrentFrameIndex()];

		const BindingInfo &info = m_StorageImageBindingInfos.at(name);

		m_Device->ImmediateSubmit(
			[&](VkCommandBuffer cmd)
			{
				for (uint32_t i = 0; i < textureVk->GetSpecification().MipLevels; i++)
				{
					m_Device->TransitionVulkanImageLayout(cmd,
														  textureVk->GetImage(),
														  i,
														  0,
														  textureVk->GetImageLayout(0, i),
														  VK_IMAGE_LAYOUT_GENERAL,
														  VK_IMAGE_ASPECT_COLOR_BIT);
					textureVk->SetImageLayout(0, i, VK_IMAGE_LAYOUT_GENERAL);
				}
			});

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageView				= textureVk->GetImageView();
		imageInfo.imageLayout			= VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstBinding			= info.Binding;
		writeDescriptorSet.dstSet				= descriptorSets.at(info.Set);
		writeDescriptorSet.descriptorCount		= 1;
		writeDescriptorSet.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.pImageInfo			= &imageInfo;

		vkUpdateDescriptorSets(m_Device->GetVkDevice(), 1, &writeDescriptorSet, 0, nullptr);

		m_BoundStorageImages[name] = view;
	}

	const std::map<uint32_t, VkDescriptorSetLayout> &ResourceSetVk::GetDescriptorSetLayouts() const
	{
		return m_DescriptorSetLayouts;
	}

	const std::vector<std::map<uint32_t, VkDescriptorSet>> &ResourceSetVk::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}
}	 // namespace Nexus::Graphics

#endif