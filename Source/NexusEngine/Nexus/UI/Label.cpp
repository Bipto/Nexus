#include "Label.hpp"

namespace Nexus::UI
{
    void Label::Update()
    {
    }

    void Label::Render(Graphics::BatchRenderer *batchRenderer)
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

        Nexus::Graphics::Rectangle rect(m_Position, m_Size);

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(rect, m_BackgroundColour);

        if (m_Font)
        {
            batchRenderer->DrawString(m_Text, {m_Position.X, m_Position.Y}, 28, m_ForegroundColour, m_Font);
        }

        batchRenderer->End();
    }

    void Label::SetText(const std::string &text)
    {
        m_Text = text;
    }

    const std::string &Label::GetText() const
    {
        return m_Text;
    }
}