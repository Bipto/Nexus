#include "Label.hpp"

namespace Nexus::UI
{
    void Label::Render(Graphics::BatchRenderer *batchRenderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Scissor scissor = GetScissor();
        Nexus::Graphics::Rectangle<float> rect = GetRectangle();

        batchRenderer->Begin(vp, scissor);

        batchRenderer->DrawQuadFill(rect, m_BackgroundColour);

        if (m_Font)
        {
            batchRenderer->DrawString(m_Text, {rect.GetLeft() + m_MarginLeft, rect.GetTop() + m_MarginTop}, m_FontSize, m_ForegroundColour, m_Font);
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
