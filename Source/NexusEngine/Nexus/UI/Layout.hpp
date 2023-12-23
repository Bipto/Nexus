#pragma once

#include "Control.hpp"
#include "Nexus/Window.hpp"

#include "Nexus/Renderer/BatchRenderer.hpp"

#include <vector>

namespace Nexus::UI
{
    class Layout
    {
    public:
        Layout() = delete;
        Layout(Window *window, Nexus::Graphics::GraphicsDevice *device);
        void Update();
        void Render();
        void AddControl(Control *control);

    private:
        std::vector<Control *> m_Controls;
        Window *m_Window = nullptr;
        Nexus::Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;
        Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
    };
}