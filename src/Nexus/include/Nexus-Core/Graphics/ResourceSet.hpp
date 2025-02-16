#pragma once

#include "Nexus-Core/Graphics/GPUBuffer.hpp"
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

		ResourceSetSpecification &operator+(const ResourceSetSpecification &other)
		{
			SampledImages.insert(SampledImages.end(), other.SampledImages.begin(), other.SampledImages.end());
			UniformBuffers.insert(UniformBuffers.end(), UniformBuffers.begin(), UniformBuffers.end());
			return *this;
		}

		ResourceSetSpecification &operator+=(const ResourceSetSpecification &other)
		{
			SampledImages.insert(SampledImages.end(), other.SampledImages.begin(), other.SampledImages.end());
			UniformBuffers.insert(UniformBuffers.end(), other.UniformBuffers.begin(), other.UniformBuffers.end());
			return *this;
		}
	};

	struct CombinedImageSampler
	{
		std::variant<WeakRef<Texture2D>, WeakRef<Cubemap>> ImageTexture = {};
		WeakRef<Sampler>								   ImageSampler = {};
	};

	class ResourceSet
	{
	  public:
		NX_API ResourceSet(const ResourceSetSpecification &spec);
		NX_API virtual ~ResourceSet()
		{
		}

		NX_API virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name)					 = 0;
		NX_API virtual void WriteCombinedImageSampler(Ref<Texture2D> texture, Ref<Sampler> sampler, const std::string &name) = 0;
		NX_API virtual void WriteCombinedImageSampler(Ref<Cubemap> cubemap, Ref<Sampler> sampler, const std::string &name)	 = 0;

		NX_API const ResourceSetSpecification &GetSpecification() const;
		NX_API static constexpr uint32_t	   DescriptorSetCount = 64;
		NX_API static uint32_t				   GetLinearDescriptorSlot(uint32_t set, uint32_t binding);

		NX_API static std::map<std::string, uint32_t> RemapToLinearBindings(const std::vector<ResourceBinding> &resources);

		NX_API const std::map<std::string, WeakRef<UniformBuffer>> &GetBoundUniformBuffers() const;
		NX_API const std::map<std::string, CombinedImageSampler> &GetBoundCombinedImageSamplers() const;

	  protected:
		ResourceSetSpecification		   m_Specification;
		std::map<std::string, BindingInfo> m_CombinedImageSamplerBindingInfos;
		std::map<std::string, BindingInfo> m_UniformBufferBindingInfos;

		std::map<std::string, WeakRef<UniformBuffer>> m_BoundUniformBuffers;
		std::map<std::string, CombinedImageSampler>	  m_BoundCombinedImageSamplers;
	};
}	 // namespace Nexus::Graphics