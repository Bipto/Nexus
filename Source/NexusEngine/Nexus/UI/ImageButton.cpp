#include "ImageButton.hpp"

namespace Nexus::UI
{
    void Nexus::UI::ImageButton::Render(Graphics::BatchRenderer *batchRenderer)
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
            glm::vec4 colour = {1.0f, 1.0f, 1.0f, 1.0f};

            if (m_Hovered)
            {
                colour = m_HoveredColour;
            }

            if (m_Pressed)
            {
                colour = m_ClickedColour;
            }

            colour *= m_Filter;

            batchRenderer->DrawQuadFill(textureRect, colour, m_Texture);
        }

        batchRenderer->End();
    }

    void ImageButton::SetTexture(const Ref<Graphics::Texture> texture)
    {
        m_Texture = texture;
    }

    const Ref<Graphics::Texture> ImageButton::GetTexture() const
    {
        return m_Texture;
    }

    void ImageButton::SetFilter(const glm::vec4 &filter)
    {
        m_Filter = filter;
    }

    const glm::vec4 &ImageButton::GetFilter() const
    {
        return m_Filter;
    }
}