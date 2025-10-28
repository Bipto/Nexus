#include "Nexus-Core/Graphics/ResourceSet.hpp"

#include "Nexus-Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
	IResourceSet::IResourceSet(Ref<Pipeline> pipeline) : m_Pipeline(pipeline)
	{
		m_ShaderResources = pipeline->GetRequiredShaderResources();
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

	const ResourceSetDescriptors &IResourceSet::GetBoundResources() const
	{
		return m_BoundResources;
	}
}	 // namespace Nexus::Graphics
