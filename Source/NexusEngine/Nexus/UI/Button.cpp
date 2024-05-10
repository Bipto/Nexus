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
        // scissor.X = m_Position.X;
        // scissor.Y = m_Position.Y;
        // scissor.Width = m_Size.X;
        // scissor.Height = m_Size.Y;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = canvas->GetSize().X;
        scissor.Height = canvas->GetSize().Y;

        Nexus::Graphics::Rectangle<float> rect = GetRectangle();

        batchRenderer->Begin(vp, scissor);

        glm::vec4 color = m_BackgroundColour;

        if (m_Hovered)
        {
            color *= 0.5f;
        }

        // batchRenderer->DrawQuadFill(rect, color);

        const auto size = m_Font->MeasureString(m_Text, m_FontSize);
        Nexus::Graphics::Rectangle<float> textRect = {(float)m_Position.X, (float)m_Position.Y, (float)size.X, (float)size.Y};

        if (m_Font)
        {
            batchRenderer->DrawQuadFill(textRect, {1.0f, 0.0f, 0.0f, 1.0f});
            batchRenderer->DrawString(m_Text, {rect.GetLeft(), rect.GetTop()}, m_FontSize, m_ForegroundColour, m_Font);
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