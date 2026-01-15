#pragma once

#include "Nexus-Core/Graphics/ISurface.hpp"
#include "Platform/OpenGL/Context/IViewContext.hpp"
#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	class SurfaceOpenGL : public ISurface
	{
	  public:
		virtual ~SurfaceOpenGL() = default;
		virtual std::unique_ptr<GL::IViewContext> CreateOpenGLContext(GraphicsDeviceOpenGL *device, const GL::ContextDescription &desc) const = 0;
	};
}	 // namespace Nexus::Graphics