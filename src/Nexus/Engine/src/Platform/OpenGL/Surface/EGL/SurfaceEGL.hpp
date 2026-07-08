#pragma once

#include "Platform/OpenGL/ContextDescription.hpp"
#include "Platform/OpenGL/Surface/SurfaceOpenGL.hpp"

namespace Nexus::Graphics
{
    class SurfaceEGL final : public SurfaceOpenGL
    {
      public:
        SurfaceEGL(uintptr_t display, uint32_t screen, uint32_t window, const GraphicsDeviceOpenGL *device);
        ~SurfaceEGL() override = default;
        std::unique_ptr<GL::IViewContext> CreateOpenGLContext(
            GraphicsDeviceOpenGL *device, const GL::ContextDescription &desc
        ) const final;

      private:
        uintptr_t m_Display = {};
        uint32_t m_Screen = {};
        uint32_t m_Window = {};
        const GraphicsDeviceOpenGL *m_Device = nullptr;
    };
} // namespace Nexus::Graphics