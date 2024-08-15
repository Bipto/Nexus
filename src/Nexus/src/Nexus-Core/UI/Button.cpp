#include "Button.hpp"

namespace Nexus::UI
{
void Nexus::UI::Button::OnUpdate()
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

void Nexus::UI::Button::OnRender(Nexus::Graphics::BatchRenderer *renderer)
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
    Nexus::Graphics::RoundedRectangle rrect(
        {rect.GetLeft() + m_BorderThickness, rect.GetTop() + m_BorderThickness, rect.GetWidth() - (m_BorderThickness * 2), rect.GetHeight() - (m_BorderThickness * 2)},
        m_CornerRounding, m_CornerRounding, m_CornerRounding, m_CornerRounding);
    Nexus::Graphics::Rectangle<float> drawable = GetContentBounds();

    Nexus::Graphics::Scissor scissor = GetScissorRectangle();

    renderer->Begin(vp, scissor);

    if (m_ContainsMouse)
    {
        renderer->DrawRoundedRectangleFill(rrect, m_HoveredColour);
    }
    else
    {
        renderer->DrawRoundedRectangleFill(rrect, m_BackgroundColour);
    }

    if (m_BorderThickness)
    {
        Nexus::Graphics::RoundedRectangle border(rrect.GetLeft() + m_BorderThickness, rrect.GetTop() + m_BorderThickness, rrect.GetWidth() - m_BorderThickness,
                                                 rrect.GetHeight() - m_BorderThickness);
        renderer->DrawRoundedRectangle(rrect, m_BorderColour, m_BorderThickness);
    }

    if (m_Font)
    {
        renderer->DrawString(m_Text, {drawable.GetLeft(), drawable.GetTop()}, m_FontSize, m_ForegroundColour, m_Font);
    }

    renderer->End();
}

void Button::OnAutoSize()
{
    if (m_Font)
    {
        Nexus::Point2D<float> size = m_Font->MeasureString(m_Text, m_FontSize);
        m_Size = size;
    }
}

void Button::HandleMouseClick(const MouseClick &e)
{
}

void Button::SetText(const std::string &text)
{
    m_Text = text;
}

void Button::SetHoveredColour(const glm::vec4 &colour)
{
    m_HoveredColour = colour;
}

const std::string &Button::GetText() const
{
    return m_Text;
}

const glm::vec4 Button::GetHoveredColour() const
{
    return m_HoveredColour;
}
} // namespace Nexus::UI