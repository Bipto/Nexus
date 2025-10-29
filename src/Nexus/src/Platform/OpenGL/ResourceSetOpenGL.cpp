#if defined(NX_PLATFORM_OPENGL)

	#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
	ResourceSetOpenGL::ResourceSetOpenGL(Ref<Pipeline> pipeline) : IResourceSet(pipeline)
	{
	}

	void ResourceSetOpenGL::WriteStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)
	{
		m_BoundStorageBuffers[name] = storageBuffer;
	}

	void ResourceSetOpenGL::WriteDynamicStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
		m_BoundUniformBuffers[name] = uniformBuffer;
	}

	void ResourceSetOpenGL::WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name)
	{
		m_BoundCombinedImageSamplers[name] = combinedImageSampler;
	}

	void ResourceSetOpenGL::WriteSampledImage(Ref<ITextureView> textureView, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteSampler(Ref<ISampler> sampler, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructure, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteStorageImage(const StorageImageView &view, const std::string &name)
	{
		m_BoundStorageImages[name] = view;
	}

	void ResourceSetOpenGL::WriteUniformTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name)
	{
	}

	void ResourceSetOpenGL::WriteStorageTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name)
	{
	}

	void ResourceSetOpenGL::Flush()
	{
	}

}	 // namespace Nexus::Graphics

#endif