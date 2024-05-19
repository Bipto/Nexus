#include "Button.hpp"

namespace Nexus::UI
{
    void Button::Render(Graphics::BatchRenderer *batchRenderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> rect = GetRectangle();
        Nexus::Graphics::Scissor scissor = GetScissor();

        batchRenderer->Begin(vp, scissor);

        glm::vec4 colour = m_BackgroundColour;

        if (m_Hovered)
        {
            colour = m_HoveredColour;
        }

        if (m_Pressed)
        {
            colour = m_ClickedColour;
        }

        if (m_Font)
        {
            batchRenderer->DrawQuadFill(rect, colour);
            batchRenderer->DrawQuad(rect, {0.0f, 0.0f, 0.0f, 1.0f}, m_BorderThickness);
            batchRenderer->DrawString(m_Text, {rect.GetLeft() + m_MarginLeft, rect.GetTop() + m_MarginTop}, m_FontSize, m_ForegroundColour, m_Font);
        }

        batchRenderer->End();
    }

    void Button::SetText(const std::string &text)
    {
        m_Text = text;
    }

    const std::string &Button::GetText() const
    {
        return m_Text;
    }

    void Button::SetBorderThickness(uint32_t thickness)
    {
        m_BorderThickness = thickness;
    }

    const uint32_t Button::GetBorderThickness() const
    {
        return m_BorderThickness;
    }
}