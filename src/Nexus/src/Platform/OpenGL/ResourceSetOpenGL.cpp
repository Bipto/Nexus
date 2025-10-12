#if defined(NX_PLATFORM_OPENGL)

	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	ResourceSetOpenGL::ResourceSetOpenGL(Ref<Pipeline> pipeline) : ResourceSet(pipeline)
	{
	}

	void ResourceSetOpenGL::WriteStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)
	{
		m_BoundStorageBuffers[name] = storageBuffer;
	}

	void ResourceSetOpenGL::WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
		m_BoundUniformBuffers[name] = uniformBuffer;
	}

	void ResourceSetOpenGL::WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name)
	{
		m_BoundCombinedImageSamplers[name] = combinedImageSampler;
	}

	void ResourceSetOpenGL::WriteStorageImage(const StorageImageView &view, const std::string &name)
	{
		m_BoundStorageImages[name] = view;
	}

}	 // namespace Nexus::Graphics

#endif