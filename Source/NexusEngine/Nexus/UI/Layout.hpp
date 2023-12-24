#pragma once

#include "Control.hpp"

#include "Nexus/Window.hpp"
#include "Nexus/UI/Window.hpp"

#include "Nexus/Graphics/Font.hpp"

#include "Nexus/Renderer/BatchRenderer.hpp"

#include <vector>

namespace Nexus::UI
{
    class Layout
    {
    public:
        Layout() = delete;
        Layout(Nexus::Window *window, Nexus::Graphics::GraphicsDevice *device);
        void Update();
        void Render();
        void AddControl(Control *control);
        UI::Window *AddWindow(const std::string &title, int x, int y, int width, int height, Nexus::Graphics::Font *font);

    private:
        std::vector<Control *> m_Controls;
        std::vector<UI::Window *> m_Windows;
        Nexus::Window *m_Window = nullptr;
        Nexus::Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;
        Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
    };
}