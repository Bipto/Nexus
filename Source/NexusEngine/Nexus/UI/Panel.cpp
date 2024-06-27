#include "Panel.hpp"

namespace Nexus::UI
{
    void Panel::OnUpdate()
    {
        for (Control *ctrl : m_Controls)
        {
            ctrl->OnUpdate();
        }
    }

    void Panel::OnRender(Graphics::BatchRenderer *renderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = canvas->GetPosition().X;
        vp.Y = canvas->GetPosition().Y;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> rect = GetControlBounds();
        Nexus::Graphics::RoundedRectangle rrect({rect.GetLeft() + m_BorderThickness, rect.GetTop() + m_BorderThickness, rect.GetWidth() - (m_BorderThickness * 2), rect.GetHeight() - (m_BorderThickness * 2)}, m_CornerRounding, m_CornerRounding, m_CornerRounding, m_CornerRounding);

        Nexus::Graphics::Scissor scissor = GetScissorRectangle();

        renderer->Begin(vp, scissor);

        renderer->DrawRoundedRectangleFill(rrect, m_BackgroundColour);

        renderer->End();

        for (Control *ctrl : m_Controls)
        {
            ctrl->OnRender(renderer);
        }
    }

    void Panel::OnAutoSize()
    {
    }

    void Panel::HandleMouseClick(const MouseClick &e)
    {
    }
}