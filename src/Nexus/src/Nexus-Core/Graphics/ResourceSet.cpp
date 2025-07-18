#include "Nexus-Core/Graphics/ResourceSet.hpp"

namespace Nexus::Graphics
{
	ResourceSet::ResourceSet(const ResourceSetSpecification &spec) : m_Description(spec)
	{
		for (const auto &texture : spec.SampledImages)
		{
			BindingInfo info;
			info.Set										 = texture.Set;
			info.Binding									 = texture.Binding;
			m_CombinedImageSamplerBindingInfos[texture.Name] = info;
		}

		for (const auto &uniformBuffer : spec.UniformBuffers)
		{
			BindingInfo info;
			info.Set										= uniformBuffer.Set;
			info.Binding									= uniformBuffer.Binding;
			m_UniformBufferBindingInfos[uniformBuffer.Name] = info;
		}

		for (const auto& storageImage : spec.StorageImages)
		{
			BindingInfo info;
			info.Set = storageImage.Set;
			info.Binding = storageImage.Binding;
			m_StorageImageBindingInfos[storageImage.Name] = info;
		}

		for (const auto &storageBuffer : spec.StorageBuffers)
		{
			BindingInfo info;
			info.Set										= storageBuffer.Set;
			info.Binding									= storageBuffer.Binding;
			m_StorageBufferBindingInfos[storageBuffer.Name] = info;
		}
	}

	const ResourceSetSpecification &ResourceSet::GetDescription() const
	{
		return m_Description;
	}

	uint32_t ResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t binding)
	{
		return (set * DescriptorSetCount) + binding;
	}

	std::map<std::string, uint32_t> ResourceSet::RemapToLinearBindings(const std::vector<ResourceBinding> &resources)
	{
		uint32_t						resourceIndex = 0;
		std::map<std::string, uint32_t> bindings;

		for (const auto &resource : resources)
		{
			uint32_t remappedBinding = GetLinearDescriptorSlot(resource.Set, resource.Binding);
			bindings[resource.Name]	 = resourceIndex++;
		}

		return bindings;
	}

	const std::map<std::string, UniformBufferView> &ResourceSet::GetBoundUniformBuffers() const
	{
		return m_BoundUniformBuffers;
	}

	const std::map<std::string, CombinedImageSampler> &ResourceSet::GetBoundCombinedImageSamplers() const
	{
		return m_BoundCombinedImageSamplers;
	}

	const std::map<std::string, StorageImageView> &ResourceSet::GetBoundStorageImages() const
	{
		return m_BoundStorageImages;
	}

	const std::map<std::string, StorageBufferView> &Nexus::Graphics::ResourceSet::GetBoundStorageBuffers() const
	{
		return m_BoundStorageBuffers;
	}
}	 // namespace Nexus::Graphics
