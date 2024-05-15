#include "PictureBox.hpp"

namespace Nexus::UI
{
    void PictureBox::Render(Graphics::BatchRenderer *batchRenderer)
    {
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
        Nexus::Graphics::Rectangle<float> textureRect = Nexus::Graphics::Rectangle<float>(rect.GetLeft() + m_MarginLeft,
                                                                                          rect.GetTop() + m_MarginTop,
                                                                                          rect.GetWidth() - m_MarginRight - m_MarginLeft,
                                                                                          rect.GetHeight() - m_MarginBottom - m_MarginTop);

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(rect, m_BackgroundColour);

        if (m_Texture)
        {
            batchRenderer->DrawQuadFill(textureRect, {1.0f, 1.0f, 1.0f, 1.0f}, m_Texture);
        }

        batchRenderer->End();
    }

    void PictureBox::SetTexture(const Nexus::Ref<Nexus::Graphics::Texture> texture)
    {
        m_Texture = texture;
    }

    const Nexus::Ref<Nexus::Graphics::Texture> PictureBox::GetTexture() const
    {
        return m_Texture;
    }
}