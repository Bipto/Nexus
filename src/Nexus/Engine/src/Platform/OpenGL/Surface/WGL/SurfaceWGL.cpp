#include "SurfaceWGL.hpp"

#include "Platform/OpenGL/Context/WGL/OffscreenContextWGL.hpp"
#include "Platform/OpenGL/Context/WGL/ViewContextWGL.hpp"
#include "Platform/OpenGL/GL.hpp"

namespace Nexus::Graphics
{
    SurfaceWGL::SurfaceWGL(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance, const GraphicsDeviceOpenGL *device)
        : m_HWND(hwnd), m_HDC(hdc), m_Hinstance(hinstance), m_Device(device)
    {
    }

    std::unique_ptr<GL::IViewContext> SurfaceWGL::CreateOpenGLContext(
        GraphicsDeviceOpenGL *device, const GL::ContextDescription &desc
    ) const
    {
        GL::OffscreenContextWGL *pbufferWGL = (GL::OffscreenContextWGL *)device->GetOffscreenContext();
        HWND hwnd = reinterpret_cast<HWND>(m_HWND);
        HDC hdc = reinterpret_cast<HDC>(m_HDC);

        auto context = std::make_unique<GL::ViewContextWGL>(hwnd, hdc, pbufferWGL, desc);
        return context;
    }
} // namespace Nexus::Graphics