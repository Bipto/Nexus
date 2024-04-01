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

        Nexus::Graphics::Rectangle<float> rect((float)m_Position.X, (float)m_Position.Y, (float)m_Size.X, (float)m_Size.Y);

        batchRenderer->Begin(vp, scissor);

        glm::vec4 color = m_BackgroundColour;

        if (m_Hovered)
        {
            color *= 0.5f;
        }

        batchRenderer->DrawQuadFill(rect, color);

        if (m_Font)
        {
            batchRenderer->DrawString(m_Text, {m_Position.X, m_Position.Y}, m_Font->GetSize(), m_ForegroundColour, m_Font);
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
}