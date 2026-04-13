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
	static void CreateDynamicOffsetDataAndStageFlags(Pipeline														*pipeline,
													 std::map<uint32_t, std::vector<uint32_t>>						&offsetData,
													 std::map<std::string, ResourceSetVk::DynamicOffsetDescription> &memberOffsets,
													 VkShaderStageFlags												&pipelineStages,
													 const std::map<std::string, Nexus::Graphics::ShaderResource>	&shaderResources)
	{
		const ResourceSetDescription &resourceSetDesc = pipeline->GetResourceSetDescription();

		struct BindingData
		{
			uint32_t	ArraySize	= 0;
			std::string BindingName = "";
		};

		// this is a structure that maps set->binding->count
		std::map<uint32_t, std::map<uint32_t, BindingData>> descriptorMap = {};

		// stores the number of dynamic offsets that need to be stored per descriptor set
		std::map<uint32_t, size_t> setCounts = {};

		// iterate through all descriptors
		for (const auto &descriptor : resourceSetDesc.Descriptors)
		{
			// retrieve the reflected resource in the shader
			const ShaderResource &resource = shaderResources.at(descriptor.Name);

			// retrieve the stage flags for this descriptor and store it
			pipelineStages |= Vk::GetVkShaderStageFlagsFromShaderStages(resource.Stage);

			// if the descriptor is dynamic, we need to work the position of it in the offsets array
			if (descriptor.Type == ResourceDescriptorType::DynamicUniformBuffer || descriptor.Type == ResourceDescriptorType::DynamicStorageBuffer)
			{
				BindingData &bindingData = descriptorMap[resource.Set][resource.Binding];
				bindingData.BindingName	 = descriptor.Name;
				bindingData.ArraySize	 = descriptor.CountOrSizeInBytes;

				setCounts[resource.Set] += descriptor.CountOrSizeInBytes;
			}
		}

		size_t totalDynamicBindingCount = 0;

		// calculate the correct locations for the dynamic offsets
		for (const auto &[setIndex, bindings] : descriptorMap)
		{
			for (const auto &[bindingIndex, bindingData] : bindings)
			{
				ResourceSetVk::DynamicOffsetDescription &vulkanOffsetDesc = memberOffsets[bindingData.BindingName];
				vulkanOffsetDesc.Set									  = setIndex;
				vulkanOffsetDesc.Offset									  = totalDynamicBindingCount;

				totalDynamicBindingCount += bindingData.ArraySize;
			}
		}

		for (const auto &[setIndex, dynamicOffsetCount] : setCounts) { offsetData[setIndex].resize(dynamicOffsetCount); }
	}

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
		CreateDynamicOffsetDataAndStageFlags(pipeline.get(), m_DynamicOffsets, m_DynamicOffsetMap, m_PipelineStages, m_ShaderResources);
	}

	ResourceSetVk::~ResourceSetVk()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.DestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
	}

	static void GenerateWriteBufferDescriptor(const std::string								&resourceName,
											  std::map<std::string, VkDescriptorBufferInfo> &buffersToWrite,
											  std::vector<VkWriteDescriptorSet>				&descriptorSetWrites,
											  VkBuffer										 buffer,
											  size_t										 offset,
											  size_t										 size,
											  VkDescriptorType								 descriptorType,
											  uint32_t										 binding,
											  uint32_t										 arrayElement,
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
														  uint32_t															arrayElement,
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
		descriptorInfo.dstArrayElement		 = arrayElement;
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
											 uint32_t									   arrayElement,
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
		descriptorInfo.dstArrayElement		 = arrayElement;
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
		uint32_t															 arrayElement,
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
		descriptorInfo.dstArrayElement		 = arrayElement;
		descriptorInfo.descriptorCount		 = 1;
		descriptorInfo.descriptorType		 = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	}

	static void GenerateWriteTexelBufferDescriptor(const std::string				 &resourceName,
												   std::vector<VkWriteDescriptorSet> &descriptorSetWrites,
												   VkBufferView						  bufferView,
												   uint32_t							  binding,
												   VkDescriptorSet					  descriptorSet,
												   VkDescriptorType					  descriptorType,
												   uint32_t							  arrayElement)
	{
		VkWriteDescriptorSet &descriptorInfo = descriptorSetWrites.emplace_back();
		descriptorInfo.sType				 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.pNext				 = nullptr;
		descriptorInfo.dstSet				 = descriptorSet;
		descriptorInfo.dstBinding			 = binding;
		descriptorInfo.dstArrayElement		 = arrayElement;
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
		for (const auto &[name, views] : m_QueuedResources.UniformBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteBufferDescriptor(resource.Name,
												  buffersToWrite,
												  descriptorSetWrites,
												  buffer->GetVkBuffer(),
												  view.Offset,
												  view.Size,
												  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
												  resource.Binding,
												  arrayIndex,
												  m_DescriptorSets.at(resource.Set));

					m_BoundResources.UniformBuffers[name][arrayIndex] = view;
				}
			}
		}

		// dynamic uniform buffers
		for (const auto &[name, views] : m_QueuedResources.DynamicUniformBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteBufferDescriptor(resource.Name,
												  buffersToWrite,
												  descriptorSetWrites,
												  buffer->GetVkBuffer(),
												  view.Offset,
												  view.Size,
												  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
												  arrayIndex,
												  resource.Binding,
												  m_DescriptorSets.at(resource.Set));

					m_BoundResources.DynamicUniformBuffers[name][arrayIndex] = view;
				}
			}
		}

		// inline uniform block
		for (const auto &[name, inlineData] : m_QueuedResources.InlineUniformBlocks)
		{
			const ShaderResource &resource = m_ShaderResources.at(name);

			GenerateWriteInlineUniformBlockDescriptor(resource.Name,
													  inlineBlocksToWrite,
													  descriptorSetWrites,
													  inlineData.data(),
													  inlineData.size(),
													  resource.Binding,
													  0,
													  m_DescriptorSets.at(resource.Set));

			m_BoundResources.InlineUniformBlocks[name] = inlineData;
		}

		// storage buffers
		for (const auto &[name, views] : m_QueuedResources.StorageBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteBufferDescriptor(resource.Name,
												  buffersToWrite,
												  descriptorSetWrites,
												  buffer->GetVkBuffer(),
												  view.Offset,
												  view.SizeInBytes,
												  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
												  resource.Binding,
												  arrayIndex,
												  m_DescriptorSets.at(resource.Set));

					m_BoundResources.StorageBuffers[name][arrayIndex] = view;
				}
			}
		}

		// dynamic storage buffers
		for (const auto &[name, views] : m_QueuedResources.DynamicStorageBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
			{
				const auto &view = views[arrayIndex];
				if (Ref<DeviceBufferVk> buffer = std::dynamic_pointer_cast<DeviceBufferVk>(view.BufferHandle))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteBufferDescriptor(resource.Name,
												  buffersToWrite,
												  descriptorSetWrites,
												  buffer->GetVkBuffer(),
												  view.Offset,
												  view.SizeInBytes,
												  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
												  resource.Binding,
												  arrayIndex,
												  m_DescriptorSets.at(resource.Set));

					m_BoundResources.DynamicStorageBuffers[name][arrayIndex] = view;
				}
			}
		}

		// storage images
		for (const auto &[name, storageImages] : m_QueuedResources.StorageImages)
		{
			for (size_t arrayIndex = 0; arrayIndex < storageImages.size(); arrayIndex++)
			{
				const auto &storageImage = storageImages[arrayIndex];
				if (const TextureVk *texture = dynamic_cast<const TextureVk *>(storageImage.TextureHandle.GetResource()))
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
												 arrayIndex,
												 m_DescriptorSets.at(resource.Set));

					m_BoundResources.StorageImages[name][arrayIndex] = storageImage;
				}
			}
		}

		// combined image samplers
		for (const auto &[name, combinedImageSamplers] : m_QueuedResources.CombinedImageSamplers)
		{
			for (size_t arrayIndex = 0; arrayIndex < combinedImageSamplers.size(); arrayIndex++)
			{
				const auto &combinedImageSampler = combinedImageSamplers[arrayIndex];

				const TextureViewVk *textureView = dynamic_cast<const TextureViewVk *>(combinedImageSampler.ImageTexture.GetResource());
				const SamplerVk		*sampler	 = dynamic_cast<const SamplerVk *>(combinedImageSampler.ImageSampler.GetResource());
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
												 arrayIndex,
												 m_DescriptorSets.at(resource.Set));

					m_BoundResources.CombinedImageSamplers[name][arrayIndex] = combinedImageSampler;
				}
			}
		}

		// sampled images
		for (const auto &[name, sampledImages] : m_QueuedResources.SampledImages)
		{
			for (size_t arrayIndex = 0; arrayIndex < sampledImages.size(); arrayIndex++)
			{
				const auto &sampledImage = sampledImages[arrayIndex];

				if (const TextureViewVk *textureView = dynamic_cast<const TextureViewVk *>(sampledImage.GetResource()))
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
												 arrayIndex,
												 m_DescriptorSets.at(resource.Set));

					m_BoundResources.SampledImages[name][arrayIndex] = sampledImage;
				}
			}
		}

		// samplers
		for (const auto &[name, samplers] : m_QueuedResources.Samplers)
		{
			for (size_t arrayIndex = 0; arrayIndex < samplers.size(); arrayIndex++)
			{
				const auto &sampler = samplers[arrayIndex];

				if (const SamplerVk *samplerVk = dynamic_cast<const SamplerVk *>(sampler.GetResource()))
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
												 arrayIndex,
												 m_DescriptorSets.at(resource.Set));

					m_BoundResources.Samplers[name][arrayIndex] = sampler;
				}
			}
		}

		// acceleration structures
		for (const auto &[name, accelerationStructures] : m_QueuedResources.AccelerationStructures)
		{
			for (size_t arrayIndex = 0; arrayIndex < accelerationStructures.size(); arrayIndex++)
			{
				const auto &accelerationStructure = accelerationStructures[arrayIndex];
				if (Ref<AccelerationStructureVk> accelerationStructureVk = std::dynamic_pointer_cast<AccelerationStructureVk>(accelerationStructure))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteAccelerationStructureDescriptor(name,
																 accelerationStructuresToWrite,
																 descriptorSetWrites,
																 accelerationStructureVk->GetHandle(),
																 resource.Binding,
																 arrayIndex,
																 m_DescriptorSets.at(resource.Set));

					m_BoundResources.AccelerationStructures[name][arrayIndex] = accelerationStructure;
				}
			}
		}

		// uniform texel buffers
		for (const auto &[name, texelBuffers] : m_QueuedResources.UniformTexelBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
			{
				const auto &texelBuffer = texelBuffers[arrayIndex];
				if (Ref<TexelBufferVk> texelBufferVk = std::dynamic_pointer_cast<TexelBufferVk>(texelBuffer))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteTexelBufferDescriptor(resource.Name,
													   descriptorSetWrites,
													   texelBufferVk->GetVkBufferView(),
													   resource.Binding,
													   m_DescriptorSets.at(resource.Set),
													   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
													   arrayIndex);

					m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
				}
			}
		}

		// storage texel buffers
		for (const auto &[name, texelBuffers] : m_QueuedResources.StorageTexelBuffers)
		{
			for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
			{
				const auto &texelBuffer = texelBuffers[arrayIndex];
				if (Ref<TexelBufferVk> texelBufferVk = std::dynamic_pointer_cast<TexelBufferVk>(texelBuffer))
				{
					const ShaderResource &resource = m_ShaderResources.at(name);

					GenerateWriteTexelBufferDescriptor(resource.Name,
													   descriptorSetWrites,
													   texelBufferVk->GetVkBufferView(),
													   resource.Binding,
													   m_DescriptorSets.at(resource.Set),
													   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
													   arrayIndex);

					m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
				}
			}
		}

		// perform the descriptor set update
		context.UpdateDescriptorSets(m_Device->GetVkDevice(), descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);
	}

	void ResourceSetVk::Bind(const GladVulkanContext							&context,
							 VkCommandBuffer									 cmd,
							 PipelineVk											*pipeline,
							 VkPipelineBindPoint								 bindPoint,
							 const std::map<std::string, std::vector<uint32_t>> &dynamicOffsets)
	{
		// iterate through all dynamic offsets and enter their values into the correct spot in the buffer
		for (const auto &[descriptorName, dynamicOffsetVector] : dynamicOffsets)
		{
			const DynamicOffsetDescription &dynamicOffsetDesc = m_DynamicOffsetMap[descriptorName];

			// maybe this should iterate through each element in the stored list rather than the submitted one to prevent crashes when accessing out
			// of bounds ????
			for (size_t dynamicOffsetIndex = 0; dynamicOffsetIndex < dynamicOffsetVector.size(); dynamicOffsetIndex++)
			{
				// retrieve the actual dynamic offset and put it into the dynamic offsets buffer at the correct spot
				uint32_t dynamicOffset															  = dynamicOffsetVector[dynamicOffsetIndex];
				m_DynamicOffsets[dynamicOffsetDesc.Set][dynamicOffsetDesc.Offset + dynamicOffset] = dynamicOffset;
			}
		}

		for (const auto &[setIndex, descriptorSet] : m_DescriptorSets)
		{
			std::vector<uint32_t> &dynamicOffsets = m_DynamicOffsets[setIndex];
			Vk::BindDescriptorSets(context,
								   cmd,
								   bindPoint,
								   pipeline->GetPipelineLayout(),
								   setIndex,
								   1,
								   &descriptorSet,
								   m_PipelineStages,
								   dynamicOffsets.data(),
								   dynamicOffsets.size());
		}
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