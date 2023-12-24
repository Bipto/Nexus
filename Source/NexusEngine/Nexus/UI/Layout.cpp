#include "Layout.hpp"

#include <algorithm>

namespace Nexus::UI
{
    Layout::Layout(Nexus::Window *window, Nexus::Graphics::GraphicsDevice *device)
        : m_Window(window), m_GraphicsDevice(device)
    {
        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {window->GetSwapchain()});
    }

    void Layout::Update()
    {
        for (auto control : m_Controls)
        {
            control->Update();
        }

        for (auto window : m_Windows)
        {
            window->Update();
        }
    }

    void Layout::Render()
    {
        for (auto control : m_Controls)
        {
            control->Render(m_BatchRenderer, {m_Window->GetSwapchain()});
        }

        for (auto window : m_Windows)
        {
            window->Render(m_BatchRenderer, {m_Window->GetSwapchain()});
        }

        for (int i = 0; i < m_Windows.size(); i++)
        {
            auto window = m_Windows[i];
            if (window->IsClosing())
            {
                m_Windows.erase(m_Windows.begin() + i);
                delete window;
                i--;
            }
        }
    }

    void Layout::AddControl(Control *control)
    {
        control->Initialise(m_Window->GetInput());
        m_Controls.emplace_back(control);
    }

    UI::Window *Layout::AddWindow(const std::string &title, int x, int y, int width, int height, Nexus::Graphics::Font *font)
    {
        auto window = new UI::Window(x, y, width, height, title, font, m_Window->GetInput());
        m_Windows.emplace_back(window);
        return window;
    }
}