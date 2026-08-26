#pragma once

#include "OffscreenContextWGL.hpp"
#include "Platform/OpenGL/Context/IViewContext.hpp"

#include "Platform/OpenGL/FramebufferOpenGL.hpp"
#include "Platform/Windows/WindowsInclude.hpp"

namespace Nexus::GL
{
    class ViewContextWGL final : public IViewContext
    {
      public:
        ViewContextWGL(HWND hwnd, HDC hdc, OffscreenContextWGL *pbuffer, const ContextDescription &spec);
        virtual ~ViewContextWGL();
        bool MakeCurrent() final;
        void Swap(Graphics::TextureHandle texture, const Graphics::SwapchainPresentDescription &presentDesc) final;
        void SetVSync(bool enabled) final;
        const ContextDescription &GetDescription() const final;
        bool Validate() final;

      private:
        HGLRC CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextDescription &spec);

      private:
        HWND m_HWND = {};
        HDC m_HDC = {};
        HGLRC m_HGLRC = {};

        OffscreenContextWGL *m_PBuffer = {};
        ContextDescription m_Description = {};
    };
} // namespace Nexus::GL