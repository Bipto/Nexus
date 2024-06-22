#include "Canvas.hpp"

#include "Control.hpp"

namespace Nexus::UI
{
    Canvas::Canvas(Graphics::GraphicsDevice *device, Graphics::Swapchain *swapchain)
    {
        m_BatchRenderer = std::make_unique<Nexus::Graphics::BatchRenderer>(device, Nexus::Graphics::RenderTarget{swapchain});
        m_Window = swapchain->GetWindow();
    }

    Canvas::~Canvas()
    {
        for (auto control : m_Controls)
        {
            delete control;
        }

        m_Controls.clear();
    }

    void Canvas::SetPosition(const Point2D<uint32_t> &position)
    {
        m_Position = position;
    }

    void Canvas::SetSize(const Point2D<uint32_t> &size)
    {
        m_Size = size;
    }

    void Canvas::SetBackgroundColour(const glm::vec4 &color)
    {
        m_BackgroundColour = color;
    }

    const Point2D<uint32_t> &Canvas::GetPosition() const
    {
        return m_Position;
    }

    const Point2D<uint32_t> &Canvas::GetSize() const
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
        control->SetCanvas(this);
        m_Controls.push_back(control);

        control->OnClose += [&](Control *control)
        {
            RemoveControl(control);
        };
    }

    void Canvas::RemoveControl(Control *control)
    {
        delete control;
        m_Controls.erase(std::remove(m_Controls.begin(), m_Controls.end(), control), m_Controls.end());
    }

    Nexus::Window *Canvas::GetWindow() const
    {
        return m_Window;
    }
}