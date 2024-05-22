#include "Scrollable.hpp"

namespace Nexus::UI
{
    Scrollable::~Scrollable()
    {
        for (auto control : m_Controls)
        {
            delete control;
        }

        m_Controls.clear();
    }

    void Scrollable::Render(Graphics::BatchRenderer *batchRenderer)
    {
        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> textureRect = GetContentRegionAvailable();

        Nexus::Graphics::Scissor scissor = GetScissor();

        batchRenderer->Begin(vp, scissor);

        // fill background
        {
            batchRenderer->DrawQuadFill(textureRect, m_BackgroundColour);
        }

        // scrollbar
        {
            Nexus::Graphics::Rectangle<float> scrollbarFillRect = Nexus::Graphics::Rectangle<float>(textureRect.GetRight() - m_ScrollbarWidth,
                                                                                                    textureRect.GetTop(),
                                                                                                    m_ScrollbarWidth,
                                                                                                    textureRect.GetHeight());
            batchRenderer->DrawQuadFill(scrollbarFillRect, {1.0f, 0.0f, 0.0f, 1.0f});

            Nexus::Graphics::Rectangle<float> scrollbarControlRect = GetScrollbar();

            batchRenderer->DrawQuadFill(scrollbarControlRect, {0.0f, 1.0f, 0.0f, 1.0f});
        }

        batchRenderer->End();

        // render children
        for (const auto control : m_Controls)
        {
            control->Render(batchRenderer);
        }
    }

    void Scrollable::Update()
    {
        Control::Update();

        HandleScroll();

        for (const auto control : m_Controls)
        {
            control->Update();
        }
    }

    void Scrollable::AddControl(Control *control)
    {
        m_Controls.push_back(control);
        control->SetParent(this);
    }

    void Scrollable::RemoveControl(Control *control)
    {
        m_Controls.erase(std::remove(m_Controls.begin(), m_Controls.end(), control), m_Controls.end());
    }

    void Scrollable::SetScrollbarWidth(float width)
    {
        m_ScrollbarWidth = width;
    }

    const float Scrollable::GetScrollbarWidth() const
    {
        return m_ScrollbarWidth;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetScrollLimits() const
    {
        Nexus::Graphics::Rectangle<float> scrollableArea = GetScrollableArea();
        scrollableArea.SetWidth(scrollableArea.GetWidth() - m_Size.X);
        scrollableArea.SetHeight(scrollableArea.GetHeight() - m_Size.Y);
        return scrollableArea;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetScrollableArea() const
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;

        for (const auto control : m_Controls)
        {
            const auto &boundingRectangle = control->GetBoundingRectangle();

            if (boundingRectangle.GetLeft() < x)
            {
                x = boundingRectangle.GetLeft();
            }

            if (boundingRectangle.GetTop() < y)
            {
                y = boundingRectangle.GetTop();
            }

            float contentWidth = boundingRectangle.GetRight();
            if (contentWidth > width)
            {
                width = contentWidth;
            }

            float contentHeight = boundingRectangle.GetBottom();
            if (contentHeight > height)
            {
                height = contentHeight;
            }
        }

        return {width, height};
    }

    void Scrollable::HandleScroll()
    {
        const auto &size = GetSize();

        const auto &contentSize = GetScrollLimits();
        const float maxScroll = glm::max<float>(contentSize.GetTop(), contentSize.GetHeight());

        if (m_Hovered)
        {
            m_ChildOffset.y -= (Nexus::Input::GetMouseScrollMovementY() * m_ScrollSpeed);

            if (m_ChildOffset.y < 0)
            {
                m_ChildOffset.y = 0;
            }

            if (m_ChildOffset.y > maxScroll)
            {
                m_ChildOffset.y = maxScroll;
            }
        }
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetScrollbar() const
    {
        const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();
        Nexus::Graphics::Rectangle<float> scrollableLimits = GetScrollableArea();

        float ratio = contentRegion.GetHeight() / scrollableLimits.GetHeight();
        float scrollbarX = contentRegion.GetRight() - m_ScrollbarWidth;
        float scrollbarY = contentRegion.GetTop() + (m_ChildOffset.y * ratio);
        float scrollbarHeight = (contentRegion.GetHeight() * ratio);

        const Nexus::Graphics::Rectangle<float> scrollbarRect = Nexus::Graphics::Rectangle<float>(scrollbarX,
                                                                                                  scrollbarY,
                                                                                                  m_ScrollbarWidth,
                                                                                                  scrollbarHeight);

        return scrollbarRect;
    }
}