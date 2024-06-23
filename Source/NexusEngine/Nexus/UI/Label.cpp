#include "Label.hpp"

namespace Nexus::UI
{
    void Nexus::UI::Label::OnUpdate()
    {
        if (m_AutoSize && m_Font)
        {
            Nexus::Point2D<float> size = m_Font->MeasureString(m_Text, m_FontSize);
            m_Size = size;
        }
    }

    void Nexus::UI::Label::OnRender(Nexus::Graphics::BatchRenderer *renderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> rect = GetControlBounds();

        Nexus::Graphics::Scissor scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = rect.GetWidth();
        scissor.Height = rect.GetHeight();

        renderer->Begin(vp, scissor);

        renderer->DrawQuadFill(rect, m_BackgroundColour);

        if (m_Font)
        {
            renderer->DrawString(m_Text, {rect.GetLeft(), rect.GetTop()}, m_FontSize, m_ForegroundColour, m_Font);

            const auto &size = m_Font->MeasureString(m_Text, m_FontSize);
            renderer->DrawQuad({m_Position, size}, {0.0f, 0.0f, 0.0f, 1.0f}, 1.0f);
        }

        renderer->End();
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