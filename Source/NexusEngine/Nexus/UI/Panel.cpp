#include "Panel.hpp"

#include <algorithm>

namespace Nexus::UI
{
    Panel::~Panel()
    {
        for (auto control : m_Controls)
        {
            delete control;
        }

        m_Controls.clear();
    }

    void Nexus::UI::Panel::Render(Graphics::BatchRenderer *batchRenderer)
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
        Nexus::Graphics::Rectangle<float> textureRect = Nexus::Graphics::Rectangle<float>(rect.GetLeft() + m_MarginLeft,
                                                                                          rect.GetTop() + m_MarginTop,
                                                                                          rect.GetWidth() - m_MarginRight - m_MarginLeft,
                                                                                          rect.GetHeight() - m_MarginBottom - m_MarginTop);

        Nexus::Graphics::Scissor scissor = GetScissor();

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(rect, m_BackgroundColour);
        batchRenderer->End();

        for (const auto control : m_Controls)
        {
            control->Render(batchRenderer);
        }
    }

    void Panel::Update()
    {
        Control::Update();

        if (m_Hovered)
        {
            m_ChildOffset.y -= (Nexus::Input::GetMouseScrollMovementY() * m_ScrollSpeed);
        }

        for (const auto control : m_Controls)
        {
            control->Update();
        }
    }

    void Panel::AddControl(Control *control)
    {
        m_Controls.push_back(control);
        control->SetParent(this);
    }

    void Panel::RemoveControl(Control *control)
    {
        m_Controls.erase(std::remove(m_Controls.begin(), m_Controls.end(), control), m_Controls.end());
    }
}