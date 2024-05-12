#include "Button.hpp"

namespace Nexus::UI
{
    void Button::Update()
    {
        Control::Update();
    }

    void Button::Render(Graphics::BatchRenderer *batchRenderer)
    {
        Control::Render(batchRenderer);

        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Scissor scissor;
        scissor.X = m_Position.X;
        scissor.Y = m_Position.Y;
        scissor.Width = m_Size.X;
        scissor.Height = m_Size.Y;

        Nexus::Graphics::Rectangle<float> rect = GetRectangle();

        batchRenderer->Begin(vp, scissor);

        glm::vec4 color = m_BackgroundColour;

        if (m_Hovered)
        {
            color *= 0.5f;
        }

        if (m_Font)
        {
            const auto size = m_Font->MeasureString(m_Text, m_FontSize);
            batchRenderer->DrawQuadFill(rect, color);
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