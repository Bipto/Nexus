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
		void Flush() final;
	};
}	 // namespace Nexus::Graphics

#endif