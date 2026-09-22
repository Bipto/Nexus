#if defined(NX_PLATFORM_WEBGL)

#include "ViewContextWebGL.hpp"
#include "OffscreenContextWebGL.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

#include "Nexus-Core/Point.hpp"

#include "Profiling/Profiler.hpp"

namespace Nexus::GL
{
    BoundingClientRect GetBoundingClientRect(const std::string &id)
    {
        emscripten::val document = emscripten::val::global("document");
        emscripten::val element = document.call<emscripten::val>("getElementById", id);
        emscripten::val window = emscripten::val::global("window");

        if (element.isUndefined() || element.isNull())
        {
            std::cout << "Could not find element" << std::endl;
            return {};
        }
        emscripten::val rect = element.call<emscripten::val>("getBoundingClientRect");

        if (rect.isUndefined() || rect.isNull())
        {
            std::cout << "Could not get bounding client rect from element" << std::endl;
            return {};
        }

        double left = rect["x"].as<double>();
        double top = rect["y"].as<double>();
        double width = rect["width"].as<double>();
        double height = rect["height"].as<double>();

        return BoundingClientRect{.Left = left, .Top = top, .Width = width, .Height = height};
    }

    ViewContextWebGL::ViewContextWebGL(const std::string &canvasName,
                                       Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
                                       const ContextDescription &spec)
        : m_Device(graphicsDevice), m_Description(spec), m_CanvasName(canvasName)
    {
    }

    ViewContextWebGL::~ViewContextWebGL()
    {
    }

    bool Nexus::GL::ViewContextWebGL::MakeCurrent()
    {
        return true;
    }

    void Nexus::GL::ViewContextWebGL::Swap(Graphics::TextureHandle texture,
                                           const Graphics::SwapchainPresentDescription &presentDesc)
    {
        NX_PROFILE_FUNCTION();

        OffscreenContextWebGL *offscreenContext = (OffscreenContextWebGL *)m_Device->GetOffscreenContext();
        std::string offscreenCanvasName = offscreenContext->GetCanvasName();

        // glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferOpenGL->GetHandle());
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        BoundingClientRect fullscreenRect = GetBoundingClientRect(offscreenCanvasName);
        BoundingClientRect viewRect = GetBoundingClientRect(m_CanvasName);

        float x = viewRect.Left;
        float y = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
        float width = viewRect.Width;
        float height = viewRect.Height;

        // glEnable(GL_SCISSOR_TEST);
        // glViewport(0, 0, textureWidth, textureHeight);
        // glScissor(x, y, textureWidth, textureHeight);

        // glBlitFramebuffer(0, 0, textureWidth, textureHeight, x, y, x + textureWidth, y + textureHeight,
        //                   GL_COLOR_BUFFER_BIT, GL_LINEAR);

        Graphics::TextureCopyDescription copyDesc = {};

        // framebuffer texture
        copyDesc.Source = texture;
        copyDesc.SourceOffset = {0, 0, 0};
        copyDesc.SourceMipLevel = 0;

        // backbuffer
        copyDesc.Destination = {};
        copyDesc.DestinationMipLevel = 0;
        copyDesc.DestinationOffset = {static_cast<int32_t>(x), static_cast<int32_t>(y), 0};

        copyDesc.Extent = {texture->GetWidth(), texture->GetHeight()};
        GL::CopyTextureToTexture(copyDesc, this);
    }

    void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
    {
    }

    const ContextDescription &Nexus::GL::ViewContextWebGL::GetDescription() const
    {
        return m_Description;
    }

    void Nexus::GL::ViewContextWebGL::HandleResize()
    {
        CreateFramebuffer();
    }

    bool ViewContextWebGL::Validate()
    {
        return true;
    }

} // namespace Nexus::GL

#endif