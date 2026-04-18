#pragma once

#include <map>

#include "Core/ResourceDeclaration.hpp"

#include "RHI/DeviceBuffer.hpp"
#include "RHI/Sampler.hpp"
#include "RHI/TextureView.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
	struct CombinedImageSampler
	{
		TextureViewHandle ImageTexture = {};
		SamplerHandle	  ImageSampler = {};
	};

	enum class ShaderAccess
	{
		Read,
		ReadWrite
	};

	struct StorageImageView
	{
		TextureHandle Texture	 = {};
		uint32_t	  ArrayLayer = 0;
		uint32_t	  MipLevel	 = 0;
		ShaderAccess  Access	 = ShaderAccess::Read;
	};

	struct StorageBufferView
	{
		DeviceBufferHandle BufferHandle = {};
		size_t			   Offset		= 0;
		size_t			   SizeInBytes	= 0;
		ShaderAccess	   Access		= ShaderAccess::Read;
	};

	struct BindingInfo
	{
		uint32_t Set	 = 0;
		uint32_t Binding = 0;
	};

	enum class ResourceDescriptorType
	{
		PushConstants,
		UniformBuffer,
		DynamicUniformBuffer,
		InlineUniformBlock,
		StorageBuffer,
		DynamicStorageBuffer,
		StorageImage,
		CombinedImageSampler,
		SampledImage,
		Sampler,
		AccelerationStructure,
		UniformTexelBuffer,
		StorageTexelBuffer
	};

	inline bool IsBuffer(ResourceDescriptorType type)
	{
		bool isBuffer = type == Graphics::ResourceDescriptorType::UniformBuffer || type == Graphics::ResourceDescriptorType::DynamicUniformBuffer ||
						type == Graphics::ResourceDescriptorType::InlineUniformBlock || type == Graphics::ResourceDescriptorType::StorageBuffer ||
						type == Graphics::ResourceDescriptorType::DynamicStorageBuffer ||
						type == Graphics::ResourceDescriptorType::UniformTexelBuffer ||
						type == Graphics::ResourceDescriptorType::StorageTexelBuffer || type == Graphics::ResourceDescriptorType::PushConstants;

		return isBuffer;
	}

	struct ResourceDescriptor
	{
		std::string			   Name				  = "Resource";
		ResourceDescriptorType Type				  = ResourceDescriptorType::UniformBuffer;
		uint32_t			   CountOrSizeInBytes = 0;
	};

	struct ResourceSetDescription
	{
		std::vector<ResourceDescriptor>					  Descriptors		= {};
		std::map<std::string, std::vector<SamplerHandle>> ImmutableSamplers = {};
	};
}	 // namespace Nexus::Graphics