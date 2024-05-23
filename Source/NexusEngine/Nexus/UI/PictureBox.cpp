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

        Nexus::Graphics::Rectangle<float> rect = GetBoundingRectangleTranslated();
        Nexus::Graphics::Rectangle<float> textureRect = GetContentRegionAvailableTranslated();

        Nexus::Graphics::Scissor scissor = GetScissor();

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(rect, m_BackgroundColour);

        if (m_Texture)
        {
            batchRenderer->DrawQuadFill(textureRect, m_Filter, m_Texture);
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

    void PictureBox::SetFilter(const glm::vec4 &filter)
    {
        m_Filter = filter;
    }

    const glm::vec4 &PictureBox::GetFilter() const
    {
        return m_Filter;
    }
}