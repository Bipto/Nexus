#pragma once

#include "Platform/OpenGL/Context/IViewContext.hpp"
#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
#include "RHI/ISurface.hpp"

namespace Nexus::Graphics
{
    class SurfaceOpenGL : public ISurface
    {
      public:
        virtual ~SurfaceOpenGL() = default;
        virtual std::unique_ptr<GL::IViewContext> CreateOpenGLContext(GraphicsDeviceOpenGL *device,
                                                                      const GL::ContextDescription &desc) const = 0;
    };
} // namespace Nexus::Graphics