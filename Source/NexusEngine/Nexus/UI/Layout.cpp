#include "Layout.hpp"

namespace Nexus::UI
{
    Layout::Layout(Window *window, Nexus::Graphics::GraphicsDevice *device)
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
    }

    void Layout::Render()
    {
        for (auto control : m_Controls)
        {
            control->Render(m_BatchRenderer, {m_Window->GetSwapchain()});
        }
    }

    void Layout::AddControl(Control *control)
    {
        control->Initialise(m_Window->GetInput());
        m_Controls.emplace_back(control);
    }
}