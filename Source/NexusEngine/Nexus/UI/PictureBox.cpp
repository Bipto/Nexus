#include "PictureBox.hpp"

namespace Nexus::UI
{
    void PictureBox::OnUpdate()
    {
        if (m_AutoSize)
        {
            OnAutoSize();
        }

        const auto &mousePos = Nexus::Input::GetMousePosition();
        Nexus::Graphics::Rectangle<float> rect = GetControlBounds();
        Nexus::Graphics::RoundedRectangle rrect(rect, m_CornerRounding, m_CornerRounding, m_CornerRounding, m_CornerRounding);

        if (rrect.Contains(mousePos.To<float>()))
        {
            if (!m_ContainsMouse)
            {
                m_ContainsMouse = true;
                OnMouseEnter.Invoke(this);
            }

            if (Nexus::Input::IsLeftMouseReleased())
            {
                OnMouseClick.Invoke(this);
            }
        }
        else
        {
            if (m_ContainsMouse)
            {
                m_ContainsMouse = false;
                OnMouseLeave.Invoke(this);
            }
        }
    }

    void PictureBox::OnRender(Nexus::Graphics::BatchRenderer *renderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = canvas->GetPosition().X;
        vp.Y = canvas->GetPosition().Y;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> rect = GetControlBounds();
        Nexus::Graphics::RoundedRectangle outlineRect(
            {rect.GetLeft() + m_BorderThickness,
             rect.GetTop() + m_BorderThickness,
             rect.GetWidth() - (m_BorderThickness * 2),
             rect.GetHeight() - (m_BorderThickness * 2)},
            m_CornerRounding, m_CornerRounding, m_CornerRounding, m_CornerRounding);

        Nexus::Graphics::Scissor scissor = GetScissorRectangle();

        renderer->Begin(vp, scissor);

        renderer->DrawRoundedRectangleFill(outlineRect, m_BackgroundColour);

        if (m_Texture)
        {
            Nexus::Graphics::RoundedRectangle textureBoundsWidthPadding(
                rect.GetLeft() + m_Padding.Left,
                rect.GetTop() + m_Padding.Top,
                rect.GetWidth() - m_Padding.Left - m_Padding.Right,
                rect.GetHeight() - m_Padding.Top - m_Padding.Bottom,
                m_CornerRounding,
                m_CornerRounding,
                m_CornerRounding,
                m_CornerRounding);

            renderer->DrawRoundedRectangleFill(textureBoundsWidthPadding, {1.0f, 1.0f, 1.0f, 1.0f}, m_Texture, 1.0f);
        }

        if (m_BorderThickness > 0.0f)
        {
            renderer->DrawRoundedRectangle(outlineRect, m_BorderColour, m_BorderThickness);
        }

        renderer->End();
    }

    void PictureBox::OnAutoSize()
    {
        if (m_Texture)
        {
            const Point2D<uint32_t> textureSize = {
                m_Texture->GetTextureSpecification().Width,
                m_Texture->GetTextureSpecification().Height};

            m_Size = textureSize.To<float>();
        }
    }

    void PictureBox::HandleMouseClick(const MouseClick &e)
    {
    }

    void PictureBox::SetTexture(Ref<Graphics::Texture> texture)
    {
        m_Texture = texture;
    }

    const Ref<Graphics::Texture> PictureBox::GetTexture() const
    {
        return m_Texture;
    }
}