#if defined(NX_PLATFORM_OPENGL)

	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec) : ResourceSet(spec)
	{
	}

	void ResourceSetOpenGL::WriteStorageBuffer(StorageBufferView storageBuffer, const std::string &name)
	{
		m_BoundStorageBuffers[name] = storageBuffer;
	}

	void ResourceSetOpenGL::WriteUniformBuffer(UniformBufferView uniformBuffer, const std::string &name)
	{
		m_BoundUniformBuffers[name] = uniformBuffer;
	}

	void ResourceSetOpenGL::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
	{
		CombinedImageSampler ciSampler {};
		ciSampler.ImageTexture			   = texture;
		ciSampler.ImageSampler			   = sampler;
		m_BoundCombinedImageSamplers[name] = ciSampler;
	}

	void ResourceSetOpenGL::WriteStorageImage(StorageImageView view, const std::string &name)
	{
		m_BoundStorageImages[name] = view;
	}

}	 // namespace Nexus::Graphics

#endif