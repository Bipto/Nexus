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
		ResourceSetOpenGL(Ref<Pipeline> pipeline, GraphicsDeviceOpenGL *device);
		void Flush() final;

		void Bind(const ResourceSetBindingDescription &bindingDesc);
		void SetPushConstants(const std::string &name, const void *data, size_t offset, size_t size);

	  private:
		std::map<std::string, std::vector<int32_t>>	   m_BindingLocations			 = {};
		std::map<std::string, Ref<DeviceBufferOpenGL>> m_EmulatedPushConstants		 = {};
		std::map<std::string, Ref<DeviceBufferOpenGL>> m_EmulatedInlineUniformBlocks = {};
	};
}	 // namespace Nexus::Graphics

#endif