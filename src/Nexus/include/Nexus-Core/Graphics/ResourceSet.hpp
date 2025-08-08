#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
#include "Nexus-Core/Graphics/Sampler.hpp"
#include "Nexus-Core/Graphics/ShaderResources.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct CombinedImageSampler
	{
		Ref<Texture> ImageTexture = {};
		Ref<Sampler> ImageSampler = {};
	};

	enum class ShaderAccess
	{
		Read,
		ReadWrite
	};

	struct StorageImageView
	{
		Ref<Texture> TextureHandle = nullptr;
		uint32_t	 ArrayLayer	   = 0;
		uint32_t	 MipLevel	   = 0;
		ShaderAccess Access		   = ShaderAccess::Read;
	};

	struct StorageBufferView
	{
		Ref<DeviceBuffer> BufferHandle	   = nullptr;
		size_t			  Offset		   = 0;
		size_t			  SizeInBytes	   = 0;
		ShaderAccess	  Access		   = ShaderAccess::Read;
	};

	struct BindingInfo
	{
		uint32_t Set	 = 0;
		uint32_t Binding = 0;
	};

	class Pipeline;

	class ResourceSet
	{
	  public:
		ResourceSet(Ref<Pipeline> pipeline);
		virtual ~ResourceSet()
		{
		}

		virtual void WriteStorageBuffer(StorageBufferView, const std::string &name)									= 0;
		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)					= 0;
		virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) = 0;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name)								= 0;

		static constexpr uint32_t		DescriptorSetCount = 64;
		static uint32_t					GetLinearDescriptorSlot(uint32_t set, uint32_t binding);

		const std::map<std::string, UniformBufferView>	  &GetBoundUniformBuffers() const;
		const std::map<std::string, CombinedImageSampler> &GetBoundCombinedImageSamplers() const;
		const std::map<std::string, StorageImageView>	  &GetBoundStorageImages() const;
		const std::map<std::string, StorageBufferView>	  &GetBoundStorageBuffers() const;

	  protected:
		WeakRef<Pipeline> m_Pipeline = {};

		std::map<std::string, Nexus::Graphics::ShaderResource> m_ShaderResources;

		std::map<std::string, UniformBufferView>	m_BoundUniformBuffers;
		std::map<std::string, CombinedImageSampler> m_BoundCombinedImageSamplers;
		std::map<std::string, StorageImageView>		m_BoundStorageImages;
		std::map<std::string, StorageBufferView>	m_BoundStorageBuffers;
	};
}	 // namespace Nexus::Graphics