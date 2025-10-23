#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/ResourceSet.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"
	#include "Platform/OpenGL/SamplerOpenGL.hpp"
	#include "Platform/OpenGL/TextureOpenGL.hpp"

namespace Nexus::Graphics
{
	class ResourceSetOpenGL : public IResourceSet
	{
	  public:
		ResourceSetOpenGL(Ref<Pipeline> pipeline);
		virtual void WriteStorageBuffer(const StorageBufferView &storageBuffer, const std::string &name) override;
		virtual void WriteUniformBuffer(const UniformBufferView &uniformBuffer, const std::string &name) override;
		virtual void WriteCombinedImageSampler(const CombinedImageSampler &combinedImageSampler, const std::string &name) override;
		virtual void WriteStorageImage(const StorageImageView &view, const std::string &name) override;
	};
}	 // namespace Nexus::Graphics

#endif