#pragma once

#if defined(NX_PLATFORM_WEBGL)

#include "Platform/OpenGL/Context/IViewContext.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::GL
{
    struct BoundingClientRect
    {
        double Left = 0;
        double Top = 0;
        double Width = 0;
        double Height = 0;
    };
    class ViewContextWebGL final : public IViewContext
    {
      public:
        ViewContextWebGL(const std::string &canvasName, Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
                         const ContextDescription &spec);
        ~ViewContextWebGL() final;
        bool MakeCurrent() final;
        void Swap(Graphics::TextureHandle texture, const Graphics::SwapchainPresentDescription &presentDesc) final;
        void SetVSync(bool enabled) final;
        const ContextDescription &GetDescription() const final;
        bool Validate() final;
        void HandleResize();

      private:
        Nexus::Graphics::GraphicsDeviceOpenGL *m_Device = nullptr;
        ContextDescription m_Description = {};
        std::string m_CanvasName = {};

        BoundingClientRect m_BoundingClientRect = {};
        inline static bool s_WindowResizeRegistered = false;
    };
}; // namespace Nexus::GL

#endif