#if defined(NX_PLATFORM_VULKAN)

	#include "ResourceSetVk.hpp"

	#include "AccelerationStructureVk.hpp"
	#include "DeviceBufferVk.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "PipelineVk.hpp"
	#include "SamplerVk.hpp"
	#include "TexelBufferVk.hpp"
	#include "TextureViewVk.hpp"
	#include "TextureVk.hpp"

namespace Nexus::Graphics
{
	ResourceSetVk::ResourceSetVk(Ref<Pipeline> pipeline, GraphicsDeviceVk *device) : IResourceSet(pipeline), m_Pipeline(pipeline), m_Device(device)
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
		m_QueuedResources.StorageBuffers[name] = storageBuffer;
	}

	void ResourceSetVk::WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
		m_QueuedResources.UniformBuffers[name] = uniformBuffer;
	}

	void ResourceSetVk::WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
		m_QueuedResources.DynamicUniformBuffers[name] = uniformBuffer;
	}

	void ResourceSetVk::WriteDynamicStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)
	{
		m_QueuedResources.DynamicStorageBuffers[name] = storageBuffer;
	}

	void ResourceSetVk::WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name)
	{
		std::vector<uint8_t> &storage = m_QueuedResources.InlineUniformBlocks[name];
		storage.resize(sizeInBytes);
		memcpy(storage.data(), data, sizeInBytes);
	}

	void ResourceSetVk::WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name)
	{
		m_QueuedResources.CombinedImageSamplers[name] = combinedImageSampler;
	}

	void ResourceSetVk::WriteStorageImage(const StorageImageView &view, const std::string &name)
	{
		m_QueuedResources.StorageImages[name] = view;
	}

	void ResourceSetVk::WriteSampledImage(Ref<ITextureView> textureView, const std::string &name)
	{
		m_QueuedResources.SampledImages[name] = textureView;
	}

	void ResourceSetVk::WriteSampler(Ref<ISampler> sampler, const std::string &name)
	{
		m_QueuedResources.Samplers[name] = sampler;
	}

	void ResourceSetVk::WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructure, const std::string &name)
	{
		m_QueuedResources.AccelerationStructures[name] = accelerationStructure;
	}

	void ResourceSetVk::WriteTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name)
	{
		m_QueuedResources.TexelBuffers[name] = texelBuffer;
	}

	static void GenerateWriteBufferDescriptor(const std::string								&resourceName,
											  std::map<std::string, VkDescriptorBufferInfo> &buffersToWrite,
											  std::vector<VkWriteDescriptorSet>				&descriptorSetWrites,
											  VkBuffer										 buffer,
											  size_t										 offset,
											  size_t										 size,
											  VkDescriptorType								 descriptorType,
											  uint32_t										 binding,
											  VkDescriptorSet								 descriptorSet)
	{
		VkDescriptorBufferInfo &bufferInfo = buffersToWrite[resourceName];
		bufferInfo.buffer				   = buffer;
		bufferInfo.offset				   = offset;
		bufferInfo.range				   = size;

		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = nullptr;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.descriptorCount		 = 1;
		descriptorInfo.descriptorType		 = descriptorType;
		descriptorInfo.pBufferInfo			 = &bufferInfo;
	}

	static void GenerateWriteInlineUniformBlockDescriptor(const std::string												   &resourceName,
														  std::map<std::string, VkWriteDescriptorSetInlineUniformBlockEXT> &blocksToWrite,
														  std::vector<VkWriteDescriptorSet>								   &descriptorSetWrites,
														  const void													   *data,
														  size_t															dataSize,
														  uint32_t															binding,
														  VkDescriptorSet													descriptorSet)
	{
		VkWriteDescriptorSetInlineUniformBlockEXT &blockInfo = blocksToWrite[resourceName];
		blockInfo.sType										 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK_EXT;
		blockInfo.pData										 = data;
		blockInfo.dataSize									 = dataSize;

		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = &blockInfo;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.descriptorCount		 = dataSize;
		descriptorInfo.descriptorType		 = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT;
	}

	static void GenerateWriteImageDescriptor(const std::string							  &resourceName,
											 std::map<std::string, VkDescriptorImageInfo> &imagesToWrite,
											 std::vector<VkWriteDescriptorSet>			  &descriptorSetWrites,
											 VkImageView								   imageView,
											 VkImageLayout								   imageLayout,
											 VkSampler									   sampler,
											 VkDescriptorType							   descriptorType,
											 uint32_t									   binding,
											 VkDescriptorSet							   descriptorSet)
	{
		VkDescriptorImageInfo &imageInfo = imagesToWrite[resourceName];
		imageInfo.imageView				 = imageView;
		imageInfo.imageLayout			 = imageLayout;
		imageInfo.sampler				 = sampler;

		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = nullptr;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.descriptorCount		 = 1;
		descriptorInfo.descriptorType		 = descriptorType;
		descriptorInfo.pImageInfo			 = &imageInfo;
	}

	static void GenerateWriteAccelerationStructureDescriptor(
		const std::string													&resourceName,
		std::map<std::string, VkWriteDescriptorSetAccelerationStructureKHR> &accelerationStructuresToWrite,
		std::vector<VkWriteDescriptorSet>									&descriptorSetWrites,
		VkAccelerationStructureKHR											 accelerationStructure,
		uint32_t															 binding,
		VkDescriptorSet														 descriptorSet)
	{
		VkWriteDescriptorSetAccelerationStructureKHR &asWrite = accelerationStructuresToWrite[resourceName];
		asWrite.sType										  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		asWrite.pNext										  = nullptr;
		asWrite.accelerationStructureCount					  = 1;
		asWrite.pAccelerationStructures						  = &accelerationStructure;

		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = &asWrite;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.descriptorCount		 = 1;
		descriptorInfo.descriptorType		 = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	}

	static void GenerateWriteTexelBufferDescriptor(const std::string				 &resourceName,
												   std::vector<VkWriteDescriptorSet> &descriptorSetWrites,
												   VkBufferView						  bufferView,
												   uint32_t							  binding,
												   VkDescriptorSet					  descriptorSet,
												   VkDescriptorType					  descriptorType)
	{
		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = nullptr;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.descriptorCount		 = 1;
		descriptorInfo.descriptorType		 = descriptorType;
		descriptorInfo.pTexelBufferView		 = &bufferView;
	}

	void ResourceSetVk::Flush()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		std::map<std::string, VkDescriptorBufferInfo>						buffersToWrite				  = {};
		std::map<std::string, VkDescriptorImageInfo>						imagesToWrite				  = {};
		std::map<std::string, VkWriteDescriptorSetInlineUniformBlockEXT>	inlineBlocksToWrite			  = {};
		std::map<std::string, VkWriteDescriptorSetAccelerationStructureKHR> accelerationStructuresToWrite = {};
		std::vector<VkWriteDescriptorSet>									descriptorSetWrites			  = {};

		// uniform buffers
		for (const auto &[name, view] : m_QueuedResources.UniformBuffers)
		{
			if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteBufferDescriptor(name,
											  buffersToWrite,
											  descriptorSetWrites,
											  buffer->GetVkBuffer(),
											  view.Offset,
											  view.Size,
											  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
											  resource.Binding,
											  m_DescriptorSets.at(resource.Set));

				m_BoundResources.UniformBuffers[name] = view;
			}
		}

		// dynamic uniform buffers
		for (const auto &[name, view] : m_QueuedResources.DynamicUniformBuffers)
		{
			if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteBufferDescriptor(name,
											  buffersToWrite,
											  descriptorSetWrites,
											  buffer->GetVkBuffer(),
											  view.Offset,
											  view.Size,
											  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
											  resource.Binding,
											  m_DescriptorSets.at(resource.Set));

				m_BoundResources.DynamicUniformBuffers[name] = view;
			}
		}

		// inline uniform block
		for (const auto &[name, inlineData] : m_QueuedResources.InlineUniformBlocks)
		{
			const ShaderResource &resource = m_ShaderResources.at(name);

			GenerateWriteInlineUniformBlockDescriptor(name,
													  inlineBlocksToWrite,
													  descriptorSetWrites,
													  inlineData.data(),
													  inlineData.size(),
													  resource.Binding,
													  m_DescriptorSets.at(resource.Set));

			m_BoundResources.InlineUniformBlocks[name] = inlineData;
		}

		// storage buffers
		for (const auto &[name, view] : m_QueuedResources.StorageBuffers)
		{
			if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteBufferDescriptor(name,
											  buffersToWrite,
											  descriptorSetWrites,
											  buffer->GetVkBuffer(),
											  view.Offset,
											  view.SizeInBytes,
											  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
											  resource.Binding,
											  m_DescriptorSets.at(resource.Set));

				m_BoundResources.StorageBuffers[name] = view;
			}
		}

		// dynamic storage buffers
		for (const auto &[name, view] : m_QueuedResources.DynamicStorageBuffers)
		{
			if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteBufferDescriptor(name,
											  buffersToWrite,
											  descriptorSetWrites,
											  buffer->GetVkBuffer(),
											  view.Offset,
											  view.SizeInBytes,
											  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
											  resource.Binding,
											  m_DescriptorSets.at(resource.Set));

				m_BoundResources.DynamicStorageBuffers[name] = view;
			}
		}

		// storage images
		for (const auto &[name, storageImage] : m_QueuedResources.StorageImages)
		{
			if (Ref<TextureVk> texture = std::dynamic_pointer_cast<TextureVk>(storageImage.TextureHandle))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				VulkanTextureViewInfo viewInfo = {};
				viewInfo.BaseMipLevel		   = storageImage.MipLevel;
				viewInfo.LevelCount			   = 1;
				viewInfo.BaseArrayLayer		   = storageImage.ArrayLayer;
				viewInfo.LayerCount			   = 1;

				GenerateWriteImageDescriptor(name,
											 imagesToWrite,
											 descriptorSetWrites,
											 texture->GetImageView(viewInfo),
											 VK_IMAGE_LAYOUT_GENERAL,
											 VK_NULL_HANDLE,
											 VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
											 resource.Binding,
											 m_DescriptorSets.at(resource.Set));

				m_BoundResources.StorageImages[name] = storageImage;
			}
		}

		// combined image samplers
		for (const auto &[name, combinedImageSampler] : m_QueuedResources.CombinedImageSamplers)
		{
			Ref<TextureViewVk> textureView = std::dynamic_pointer_cast<TextureViewVk>(combinedImageSampler.ImageTexture);
			Ref<SamplerVk>	   sampler	   = std::dynamic_pointer_cast<SamplerVk>(combinedImageSampler.ImageSampler);
			if (textureView && sampler)
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteImageDescriptor(name,
											 imagesToWrite,
											 descriptorSetWrites,
											 textureView->GetVkImageView(),
											 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											 sampler->GetSampler(),
											 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
											 resource.Binding,
											 m_DescriptorSets.at(resource.Set));

				m_BoundResources.CombinedImageSamplers[name] = combinedImageSampler;
			}
		}

		// sampled images
		for (const auto &[name, sampledImage] : m_QueuedResources.SampledImages)
		{
			if (Ref<TextureViewVk> textureView = std::dynamic_pointer_cast<TextureViewVk>(sampledImage))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteImageDescriptor(name,
											 imagesToWrite,
											 descriptorSetWrites,
											 textureView->GetVkImageView(),
											 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											 VK_NULL_HANDLE,
											 VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
											 resource.Binding,
											 m_DescriptorSets.at(resource.Set));

				m_BoundResources.SampledImages[name] = sampledImage;
			}
		}

		// samplers
		for (const auto &[name, sampler] : m_QueuedResources.Samplers)
		{
			if (Ref<SamplerVk> samplerVk = std::dynamic_pointer_cast<SamplerVk>(sampler))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteImageDescriptor(name,
											 imagesToWrite,
											 descriptorSetWrites,
											 VK_NULL_HANDLE,
											 VK_IMAGE_LAYOUT_UNDEFINED,
											 samplerVk->GetSampler(),
											 VK_DESCRIPTOR_TYPE_SAMPLER,
											 resource.Binding,
											 m_DescriptorSets.at(resource.Set));

				m_BoundResources.Samplers[name] = sampler;
			}
		}

		// acceleration structures
		for (const auto &[name, accelerationStructure] : m_QueuedResources.AccelerationStructures)
		{
			if (Ref<AccelerationStructureVk> accelerationStructureVk = std::dynamic_pointer_cast<AccelerationStructureVk>(accelerationStructure))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				GenerateWriteAccelerationStructureDescriptor(name,
															 accelerationStructuresToWrite,
															 descriptorSetWrites,
															 accelerationStructureVk->GetHandle(),
															 resource.Binding,
															 m_DescriptorSets.at(resource.Set));

				m_BoundResources.AccelerationStructures[name] = accelerationStructure;
			}
		}

		// texel buffers
		for (const auto &[name, texelBuffer] : m_QueuedResources.TexelBuffers)
		{
			if (Ref<TexelBufferVk> texelBufferVk = std::dynamic_pointer_cast<TexelBufferVk>(texelBuffer))
			{
				const ShaderResource &resource = m_ShaderResources.at(name);

				const auto &reflectedShaderResources = m_Pipeline->GetRequiredShaderResources();
				if (reflectedShaderResources.contains(name))
				{
					const auto		&reflectedResource = reflectedShaderResources.at(name);
					VkDescriptorType descriptorType	   = reflectedResource.Type == ResourceType::StorageTextureBuffer
															 ? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
															 : VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

					GenerateWriteTexelBufferDescriptor(resource.Name,
													   descriptorSetWrites,
													   texelBufferVk->GetVkBufferView(),
													   resource.Binding,
													   m_DescriptorSets.at(resource.Set),
													   descriptorType);

					m_BoundResources.TexelBuffers[name] = texelBuffer;
				}
			}
		}

		// perform the descriptor set update
		context.UpdateDescriptorSets(m_Device->GetVkDevice(), descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);

		// reset the resource queue
		m_QueuedResources = {};
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