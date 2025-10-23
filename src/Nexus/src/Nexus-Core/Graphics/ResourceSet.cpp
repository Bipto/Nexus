#include "Nexus-Core/Graphics/ResourceSet.hpp"

#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
	IResourceSet::IResourceSet(Ref<Pipeline> pipeline) : m_Pipeline(pipeline)
	{
		m_ShaderResources = pipeline->GetRequiredShaderResources();
	}

	uint32_t IResourceSet::GetLinearDescriptorSlot(uint32_t set, uint32_t binding)
	{
		return (set * DescriptorSetCount) + binding;
	}

	const std::map<std::string, UniformBufferView> &IResourceSet::GetBoundUniformBuffers() const
	{
		return m_BoundUniformBuffers;
	}

	const std::map<std::string, CombinedImageSampler> &IResourceSet::GetBoundCombinedImageSamplers() const
	{
		return m_BoundCombinedImageSamplers;
	}

	const std::map<std::string, StorageImageView> &IResourceSet::GetBoundStorageImages() const
	{
		return m_BoundStorageImages;
	}

	const std::map<std::string, StorageBufferView> &Nexus::Graphics::IResourceSet::GetBoundStorageBuffers() const
	{
		return m_BoundStorageBuffers;
	}
}	 // namespace Nexus::Graphics
