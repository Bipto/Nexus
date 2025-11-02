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
		UniformTexelBuffer,
		StorageTexelBuffer
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

	using InlineBlock = std::vector<uint8_t>;

	struct NX_API ResourceSetDescriptors
	{
		std::map<std::string, std::vector<UniformBufferView>>			UniformBuffers		   = {};
		std::map<std::string, std::vector<UniformBufferView>>			DynamicUniformBuffers  = {};
		std::map<std::string, std::vector<InlineBlock>>					InlineUniformBlocks	   = {};
		std::map<std::string, std::vector<StorageBufferView>>			StorageBuffers		   = {};
		std::map<std::string, std::vector<StorageBufferView>>			DynamicStorageBuffers  = {};
		std::map<std::string, std::vector<StorageImageView>>			StorageImages		   = {};
		std::map<std::string, std::vector<CombinedImageSampler>>		CombinedImageSamplers  = {};
		std::map<std::string, std::vector<Ref<ITextureView>>>			SampledImages		   = {};
		std::map<std::string, std::vector<Ref<ISampler>>>				Samplers			   = {};
		std::map<std::string, std::vector<Ref<IAccelerationStructure>>> AccelerationStructures = {};
		std::map<std::string, std::vector<Ref<ITexelBuffer>>>			UniformTexelBuffers	   = {};
		std::map<std::string, std::vector<Ref<ITexelBuffer>>>			StorageTexelBuffers	   = {};

		void Reset();
	};

	class Pipeline;

	class NX_API IResourceSet
	{
	  public:
		IResourceSet(Ref<Pipeline> pipeline);
		virtual ~IResourceSet();

		// single descriptors
		void WriteUniformBuffer(const UniformBufferView &uniformBuffers, const std::string &name);
		void WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffers, const std::string &name);
		void WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name);
		void WriteStorageBuffer(const StorageBufferView &views, const std::string &name);
		void WriteDynamicStorageBuffer(const StorageBufferView &storageBuffers, const std::string &name);
		void WriteStorageImage(const StorageImageView &views, const std::string &name);
		void WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSamplers, const std::string &name);
		void WriteSampledImage(Ref<ITextureView> textureViews, const std::string &name);
		void WriteSampler(Ref<ISampler> samplers, const std::string &name);
		void WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructures, const std::string &name);
		void WriteUniformTexelBuffer(Ref<ITexelBuffer> texelBuffers, const std::string &name);
		void WriteStorageTexelBuffer(Ref<ITexelBuffer> texelBuffers, const std::string &name);

		// arrays
		void WriteUniformBuffers(const UniformBufferView *uniformBuffers, const std::string &name, size_t startElement, size_t count);
		void WriteDynamicUniformBuffers(const UniformBufferView *uniformBuffers, const std::string &name, size_t startElement, size_t count);
		void WriteInlineUniformBlocks(const void *data, size_t sizeInBytes, const std::string &name, size_t startElement, size_t count);
		void WriteStorageBuffers(const StorageBufferView *views, const std::string &name, size_t startElement, size_t count);
		void WriteDynamicStorageBuffers(const StorageBufferView *storageBuffers, const std::string &name, size_t startElement, size_t count);
		void WriteStorageImages(const StorageImageView *views, const std::string &name, size_t startElement, size_t count);
		void WriteCombinedImageSamplers(const CombinedImageSampler *combinedImageSamplers,
										const std::string		   &name,
										size_t						startElement,
										size_t						count);
		void WriteSampledImages(Ref<ITextureView> *textureViews, const std::string &name, size_t startElement, size_t count);
		void WriteSamplers(Ref<ISampler> *samplers, const std::string &name, size_t startElement, size_t count);
		void WriteAccelerationStructures(Ref<IAccelerationStructure> *accelerationStructures,
										 const std::string			 &name,
										 size_t						  startElement,
										 size_t						  count);
		void WriteUniformTexelBuffers(Ref<ITexelBuffer> *texelBuffers, const std::string &name, size_t startElement, size_t count);
		void WriteStorageTexelBuffers(Ref<ITexelBuffer> *texelBuffers, const std::string &name, size_t startElement, size_t count);

		virtual void Flush() = 0;

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