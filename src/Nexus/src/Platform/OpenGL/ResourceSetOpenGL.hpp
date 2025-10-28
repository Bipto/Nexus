#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"
	#include "Platform/OpenGL/SamplerOpenGL.hpp"
	#include "Platform/OpenGL/TextureOpenGL.hpp"

namespace Nexus::Graphics
{
	class ResourceSetOpenGL final : public IResourceSet
	{
	  public:
		ResourceSetOpenGL(Ref<Pipeline> pipeline);
		void WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name) final;
		void WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name) final;
		void WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name) final;
		void WriteStorageBuffer(const StorageBufferView &view, const std::string &name) final;
		void WriteDynamicStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name) final;
		void WriteStorageImage(const StorageImageView &view, const std::string &name) final;
		void WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name) final;
		void WriteSampledImage(Ref<ITextureView> textureView, const std::string &name) final;
		void WriteSampler(Ref<ISampler> sampler, const std::string &name) final;
		void WriteAccelerationStructure(Ref<IAccelerationStructure> accelerationStructure, const std::string &name) final;
		void WriteTexelBuffer(Ref<ITexelBuffer> texelBuffer, const std::string &name) final;
		void Flush() final;
	};
}	 // namespace Nexus::Graphics

#endif