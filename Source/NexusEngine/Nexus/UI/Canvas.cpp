#include "Canvas.hpp"

#include "Control.hpp"

namespace Nexus::UI
{
    Canvas::Canvas(Graphics::GraphicsDevice *device, Graphics::Swapchain *swapchain)
    {
        m_BatchRenderer = std::make_unique<Nexus::Graphics::BatchRenderer>(device, Nexus::Graphics::RenderTarget{swapchain});
    }

    void Canvas::SetPosition(const Point<uint32_t> &position)
    {
        m_Position = position;
    }

    void Canvas::SetSize(const Point<uint32_t> &size)
    {
        m_Size = size;
    }

    void Canvas::SetBackgroundColour(const glm::vec4 &color)
    {
        m_BackgroundColour = color;
    }

    const Point<uint32_t> &Canvas::GetPosition() const
    {
        return m_Position;
    }

    const Point<uint32_t> &Canvas::GetSize() const
    {
        return m_Size;
    }

    const glm::vec4 &Canvas::GetBackgroundColour() const
    {
        return m_BackgroundColour;
    }

    void Canvas::Render() const
    {
        Nexus::Graphics::Viewport vp;
        vp.X = m_Position.X;
        vp.Y = m_Position.Y;
        vp.Width = m_Size.X;
        vp.Height = m_Size.Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Scissor scissor;
        scissor.X = m_Position.X;
        scissor.Y = m_Position.Y;
        scissor.Width = m_Size.X;
        scissor.Height = m_Size.Y;

        m_BatchRenderer->Begin(vp, scissor);
        Nexus::Graphics::Rectangle<float> rect((float)m_Position.X, (float)m_Position.Y, (float)m_Size.X, (float)m_Size.Y);
        m_BatchRenderer->DrawQuadFill(rect, m_BackgroundColour);
        m_BatchRenderer->End();

        for (const auto &control : m_Controls)
        {
            control->Update();
            control->Render(m_BatchRenderer.get());
        }
    }

    void Canvas::AddControl(Control *control)
    {
        control->m_Canvas = this;
        m_Controls.push_back(control);
    }

    void Canvas::RemoveControl(Control *control)
    {
        m_Controls.erase(std::remove(m_Controls.begin(), m_Controls.end(), control), m_Controls.end());
    }
}