#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
#include "Nexus-Core/Graphics/Sampler.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	enum class ResourceType
	{
		CombinedImageSampler,
		UniformBuffer
	};

	struct ResourceBinding
	{
		std::string	 Name;
		uint32_t	 Set;
		uint32_t	 Binding;
		ResourceType Type;
	};

	struct BindingInfo
	{
		uint32_t Set	 = 0;
		uint32_t Binding = 0;
	};

	struct ResourceSetSpecification
	{
		std::vector<ResourceBinding> SampledImages;
		std::vector<ResourceBinding> UniformBuffers;
		std::vector<ResourceBinding> StorageImages;

		ResourceSetSpecification &operator+(const ResourceSetSpecification &other)
		{
			SampledImages.insert(SampledImages.end(), other.SampledImages.begin(), other.SampledImages.end());
			UniformBuffers.insert(UniformBuffers.end(), UniformBuffers.begin(), UniformBuffers.end());
			StorageImages.insert(StorageImages.end(), StorageImages.begin(), StorageImages.end());
			return *this;
		}

		ResourceSetSpecification &operator+=(const ResourceSetSpecification &other)
		{
			SampledImages.insert(SampledImages.end(), other.SampledImages.begin(), other.SampledImages.end());
			UniformBuffers.insert(UniformBuffers.end(), other.UniformBuffers.begin(), other.UniformBuffers.end());
			StorageImages.insert(StorageImages.end(), StorageImages.begin(), StorageImages.end());
			return *this;
		}

		bool HasResources() const
		{
			return SampledImages.size() || UniformBuffers.size() > 0 || StorageImages.size() > 0;
		}
	};

	struct CombinedImageSampler
	{
		std::variant<WeakRef<Texture2D>, WeakRef<Cubemap>> ImageTexture = {};
		WeakRef<Sampler>								   ImageSampler = {};
	};

	enum class StorageImageAccess
	{
		Read,
		ReadWrite
	};

	struct StorageImageView
	{
		Texture2D		  *TextureHandle = nullptr;
		uint32_t		   Level		 = 0;
		StorageImageAccess Access		 = StorageImageAccess::Read;
	};

	class ResourceSet
	{
	  public:
		ResourceSet(const ResourceSetSpecification &spec);
		virtual ~ResourceSet()
		{
		}

		virtual void WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)					  = 0;
		virtual void WriteCombinedImageSampler(Ref<Texture2D> texture, Ref<Sampler> sampler, const std::string &name) = 0;
		virtual void WriteCombinedImageSampler(Ref<Cubemap> cubemap, Ref<Sampler> sampler, const std::string &name)	  = 0;
		virtual void WriteStorageImage(StorageImageView view, const std::string &name)								  = 0;

		const ResourceSetSpecification &GetSpecification() const;
		static constexpr uint32_t		DescriptorSetCount = 64;
		static uint32_t					GetLinearDescriptorSlot(uint32_t set, uint32_t binding);

		static std::map<std::string, uint32_t> RemapToLinearBindings(const std::vector<ResourceBinding> &resources);

		const std::map<std::string, UniformBufferView>		&GetBoundUniformBuffers() const;
		const std::map<std::string, CombinedImageSampler>	&GetBoundCombinedImageSamplers() const;
		const std::map<std::string, StorageImageView>		&GetBoundStorageImages() const;

	  protected:
		ResourceSetSpecification		   m_Specification;
		std::map<std::string, BindingInfo> m_CombinedImageSamplerBindingInfos;
		std::map<std::string, BindingInfo> m_UniformBufferBindingInfos;
		std::map<std::string, BindingInfo> m_StorageImageBindingInfos;

		std::map<std::string, UniformBufferView>	  m_BoundUniformBuffers;
		std::map<std::string, CombinedImageSampler>	  m_BoundCombinedImageSamplers;
		std::map<std::string, StorageImageView>		  m_BoundStorageImages;
	};
}	 // namespace Nexus::Graphics