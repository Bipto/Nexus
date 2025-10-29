#pragma once

#include "Nexus-Core/Graphics/AccelerationStructure.hpp"
#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
#include "Nexus-Core/Graphics/Sampler.hpp"
#include "Nexus-Core/Graphics/ShaderResources.hpp"
#include "Nexus-Core/Graphics/TexelBuffer.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Graphics/TextureView.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct CombinedImageSampler
	{
		Ref<ITextureView> ImageTexture = {};
		Ref<ISampler>	  ImageSampler = {};
		TextureLayout	  Layout	   = TextureLayout::ShaderReadOnlyOptimal;
	};

	enum class ShaderAccess
	{
		Read,
		ReadWrite
	};

	struct StorageImageView
	{
		Ref<ITexture> TextureHandle = nullptr;
		uint32_t	  ArrayLayer	= 0;
		uint32_t	  MipLevel		= 0;
		ShaderAccess  Access		= ShaderAccess::Read;
		TextureLayout Layout		= TextureLayout::General;
	};

	struct StorageBufferView
	{
		Ref<IDeviceBuffer> BufferHandle = nullptr;
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
		TexelBuffer
	};

	struct ResourceDescriptor
	{
		std::string			   Name				  = "Resource";
		ResourceDescriptorType Type				  = ResourceDescriptorType::UniformBuffer;
		uint32_t			   CountOrSizeInBytes = 0;
	};

	struct ResourceSetDescription
	{
		std::vector<ResourceDescriptor>					  Descriptors		= {};
		std::map<std::string, std::vector<Ref<ISampler>>> ImmutableSamplers = {};
	};

	struct ResourceSetDescriptors
	{
		std::map<std::string, UniformBufferView>		   UniformBuffers		  = {};
		std::map<std::string, UniformBufferView>		   DynamicUniformBuffers  = {};
		std::map<std::string, std::vector<uint8_t>>		   InlineUniformBlocks	  = {};
		std::map<std::string, StorageBufferView>		   StorageBuffers		  = {};
		std::map<std::string, StorageBufferView>		   DynamicStorageBuffers  = {};
		std::map<std::string, StorageImageView>			   StorageImages		  = {};
		std::map<std::string, CombinedImageSampler>		   CombinedImageSamplers  = {};
		std::map<std::string, Ref<ITextureView>>		   SampledImages		  = {};
		std::map<std::string, Ref<ISampler>>			   Samplers				  = {};
		std::map<std::string, Ref<IAccelerationStructure>> AccelerationStructures = {};
		std::map<std::string, Ref<ITexelBuffer>>		   UniformTexelBuffers	  = {};
		std::map<std::string, Ref<ITexelBuffer>>		   StorageTexelBuffers	  = {};
	};

	class Pipeline;

	class IResourceSet
	{
	  public:
		IResourceSet(Ref<Pipeline> pipeline);
		virtual ~IResourceSet()
		{
		}

		virtual void WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)					= 0;
		virtual void WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)				= 0;
		virtual void WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name)					= 0;
		virtual void WriteStorageBuffer(const StorageBufferView &view, const std::string &name)								= 0;
		virtual void WriteDynamicStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)				= 0;
		virtual void WriteStorageImage(const StorageImageView &view, const std::string &name)								= 0;
		virtual void WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name)	= 0;
		virtual void WriteSampledImage(Ref<ITextureView> textureView, const std::string &name)								= 0;
		virtual void WriteSampler(Ref<ISampler> sampler, const std::string &name)											= 0;
		virtual void WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructure, const std::string &name) = 0;
		virtual void WriteUniformTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name)						= 0;
		virtual void WriteStorageTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name)						= 0;
		virtual void Flush()																								= 0;

		const std::map<std::string, UniformBufferView>	  &GetBoundUniformBuffers() const;
		const std::map<std::string, CombinedImageSampler> &GetBoundCombinedImageSamplers() const;
		const std::map<std::string, StorageImageView>	  &GetBoundStorageImages() const;
		const std::map<std::string, StorageBufferView>	  &GetBoundStorageBuffers() const;
		const ResourceSetDescriptors					  &GetBoundResources() const;

	  protected:
		WeakRef<Pipeline> m_Pipeline = {};

		std::map<std::string, Nexus::Graphics::ShaderResource> m_ShaderResources;

		ResourceSetDescriptors m_BoundResources	 = {};
		ResourceSetDescriptors m_QueuedResources = {};

		std::map<std::string, UniformBufferView>	m_BoundUniformBuffers;
		std::map<std::string, CombinedImageSampler> m_BoundCombinedImageSamplers;
		std::map<std::string, StorageImageView>		m_BoundStorageImages;
		std::map<std::string, StorageBufferView>	m_BoundStorageBuffers;
	};
}	 // namespace Nexus::Graphics