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

        batchRenderer->End();

        // render children
        for (const auto control : m_Controls)
        {
            control->Render(batchRenderer);
        }

        // draw vertical scrollbar
        if (RequiresVerticalScrollbar())
        {
            batchRenderer->Begin(vp, scissor);

            Nexus::Graphics::Rectangle<float> scrollbarBackgroundRect = GetVerticalScrollbarBackgroundArea();
            batchRenderer->DrawQuadFill(scrollbarBackgroundRect, m_ScrollbarBackground);

            Nexus::Graphics::Rectangle<float> scrollbarControlRect = GetVerticalScrollbarControlArea();
            batchRenderer->DrawQuadFill(scrollbarControlRect, m_ScrollbarForeground);
            batchRenderer->DrawQuad(scrollbarControlRect, {0.35f, 0.35f, 0.35f, 1.0f}, m_ScrollbarBorderThickness);

            batchRenderer->End();
        }

        if (RequiresHorizontalScrollbar())
        {
            Nexus::Graphics::Rectangle<float> scrollbarFillRect = GetHorizontalScrollbarBackgroundArea();
            batchRenderer->Begin(vp, scissor);
            batchRenderer->DrawQuadFill(scrollbarFillRect, m_ScrollbarBackground);

            Nexus::Graphics::Rectangle<float> scrollbarControlRect = GetHorizontalScrollbarControlArea();
            batchRenderer->DrawQuadFill(scrollbarControlRect, m_ScrollbarForeground);
            batchRenderer->DrawQuad(scrollbarControlRect, {0.35f, 0.35f, 0.35f, 1.0f}, m_ScrollbarBorderThickness);

            batchRenderer->End();
        }
    }

    void Scrollable::Update()
    {
        Control::Update();

        if (RequiresVerticalScrollbar())
        {
            HandleVerticalScroll();
        }

        if (RequiresHorizontalScrollbar())
        {
            HandleHorizontalScroll();
        }

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

    void Scrollable::SetScrollbarBackgroundColour(const glm::vec4 &colour)
    {
        m_ScrollbarBackground = colour;
    }

    const glm::vec4 &Scrollable::GetScrollbarBackgroundColour() const
    {
        return m_ScrollbarBackground;
    }

    void Scrollable::SetScrollbarForegroundColour(const glm::vec4 &colour)
    {
        m_ScrollbarForeground = colour;
    }

    const glm::vec4 &Scrollable::GetScrollbarForegroundColour() const
    {
        return m_ScrollbarForeground;
    }

    void Scrollable::SetScrollbarBorderThickness(const float thickness)
    {
        m_ScrollbarBorderThickness = thickness;
    }

    const float Scrollable::GetScrollbarBorderThickness() const
    {
        return m_ScrollbarBorderThickness;
    }

    void Scrollable::SetScrollbarPadding(const float padding)
    {
        m_ScrollbarPadding = padding;
    }

    const float Scrollable::GetScrollbarPadding() const
    {
        return m_ScrollbarPadding;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetScrollLimits() const
    {
        Nexus::Graphics::Rectangle<float> scrollableArea = GetScrollableAreaInControlSpace();
        scrollableArea.SetWidth(scrollableArea.GetWidth() - m_Size.X);
        scrollableArea.SetHeight(scrollableArea.GetHeight() - m_Size.Y);
        return scrollableArea;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetScrollableAreaInControlSpace() const
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

        if (m_Size.X > width)
        {
            width = m_Size.X;
        }

        if (m_Size.Y > height)
        {
            height = m_Size.Y;
        }

        return {x, y, width, height};
    }

    void Scrollable::HandleVerticalScroll()
    {
        const auto &size = GetSize();
        const auto &contentSize = GetScrollLimits();
        const float maxScrollY = glm::max<float>(0, contentSize.GetHeight());
        const float minScrollY = glm::min<float>(0, contentSize.GetTop());

        // handle scrolling with mouse wheel
        if (m_Hovered)
        {
            m_ChildOffset.y -= (Nexus::Input::GetMouseScrollMovementY() * m_ScrollSpeed);
        }

        // handle scrolling with scroll bar
        {
            const Nexus::Graphics::Rectangle<float> verticalScrollbarRect = GetVerticalScrollbarControlArea();
            const Nexus::Point<int> mousePos = Nexus::Input::GetMousePosition();

            if (verticalScrollbarRect.Contains(mousePos.To<float>()))
            {
                if (Nexus::Input::IsLeftMousePressed())
                {
                    m_ScrollbarYGrabbed = true;
                }
            }

            if (m_ScrollbarYGrabbed)
            {
                const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();
                const Nexus::Graphics::Rectangle<float> scrollableLimits = GetScrollableAreaInControlSpace();

                float ratio = scrollableLimits.GetHeight() / contentRegion.GetHeight();
                m_ChildOffset.y += Nexus::Input::GetMouseMovement().Y * ratio;

                if (Nexus::Input::IsLeftMouseReleased())
                {
                    m_ScrollbarYGrabbed = false;
                }
            }
        }

        if (m_ChildOffset.y < minScrollY)
        {
            m_ChildOffset.y = minScrollY;
        }

        if (m_ChildOffset.y > maxScrollY)
        {
            m_ChildOffset.y = maxScrollY;
        }
    }

    void Scrollable::HandleHorizontalScroll()
    {
        const auto &size = GetSize();
        const auto &contentSize = GetScrollLimits();
        const float maxScrollX = glm::max<float>(0, contentSize.GetWidth());
        const float minScrollX = glm::min<float>(0, contentSize.GetLeft());

        if (m_Hovered)
        {
        }

        // handle scrolling with scroll bar
        {
            const Nexus::Graphics::Rectangle<float> horizontalScrollbarRect = GetHorizontalScrollbarControlArea();
            const Nexus::Point<int> mousePos = Nexus::Input::GetMousePosition();

            if (horizontalScrollbarRect.Contains(mousePos.To<float>()))
            {
                if (Nexus::Input::IsLeftMousePressed())
                {
                    m_ScrollbarXGrabbed = true;
                }
            }

            if (m_ScrollbarXGrabbed)
            {
                const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();
                const Nexus::Graphics::Rectangle<float> scrollableLimits = GetScrollableAreaInControlSpace();

                float ratio = scrollableLimits.GetWidth() / contentRegion.GetWidth();
                m_ChildOffset.x += Nexus::Input::GetMouseMovement().X * ratio;

                if (Nexus::Input::IsLeftMouseReleased())
                {
                    m_ScrollbarXGrabbed = false;
                }
            }
        }

        if (m_ChildOffset.x < minScrollX)
        {
            m_ChildOffset.x = minScrollX;
        }

        if (m_ChildOffset.x > maxScrollX)
        {
            m_ChildOffset.x = maxScrollX;
        }
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetVerticalScrollbarControlArea() const
    {
        const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();
        const Nexus::Graphics::Rectangle<float> scrollableLimits = GetScrollableAreaInControlSpace();

        const float scrollableYOffset = 0 - scrollableLimits.GetTop();

        const float ratio = contentRegion.GetHeight() / scrollableLimits.GetHeight();
        const float scrollbarX = contentRegion.GetRight() - m_ScrollbarWidth - m_ScrollbarBorderThickness - m_ScrollbarPadding;
        const float scrollbarY = contentRegion.GetTop() + (m_ChildOffset.y * ratio) + m_ScrollbarBorderThickness + m_ScrollbarPadding + (scrollableYOffset / 2) + 1;
        const float scrollbarHeight = (contentRegion.GetHeight() * ratio) - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2) - (scrollableYOffset / 2) - 1;

        Nexus::Graphics::Rectangle<float> scrollbarRect = Nexus::Graphics::Rectangle<float>(scrollbarX,
                                                                                            scrollbarY,
                                                                                            m_ScrollbarWidth,
                                                                                            scrollbarHeight);

        if (RequiresHorizontalScrollbar())
        {
            scrollbarRect.SetHeight(scrollbarRect.GetHeight() - m_ScrollbarWidth - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2));
        }

        return scrollbarRect;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetVerticalScrollbarBackgroundArea() const
    {
        const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();

        Nexus::Graphics::Rectangle<float> scrollbarFillRect = Nexus::Graphics::Rectangle<float>(
            contentRegion.GetRight() - m_ScrollbarWidth - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2),
            contentRegion.GetTop(),
            m_ScrollbarWidth + (m_ScrollbarBorderThickness * 2) + (m_ScrollbarPadding * 2),
            contentRegion.GetHeight());

        return scrollbarFillRect;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetHorizontalScrollbarControlArea() const
    {
        const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();
        const Nexus::Graphics::Rectangle<float> scrollableLimits = GetScrollableAreaInControlSpace();

        const float ratio = contentRegion.GetWidth() / scrollableLimits.GetWidth();
        const float scrollbarX = contentRegion.GetLeft() + (m_ChildOffset.x * ratio) + m_ScrollbarBorderThickness + m_ScrollbarPadding;
        const float scrollbarY = contentRegion.GetBottom() - m_ScrollbarWidth - (m_ScrollbarBorderThickness) - (m_ScrollbarPadding);
        const float scrollbarWidth = (contentRegion.GetWidth() * ratio) - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2);
        const float scrollbarHeight = m_ScrollbarWidth;

        Nexus::Graphics::Rectangle<float> scrollbarRect = Nexus::Graphics::Rectangle<float>(scrollbarX,
                                                                                            scrollbarY,
                                                                                            scrollbarWidth,
                                                                                            scrollbarHeight);

        if (RequiresVerticalScrollbar())
        {
            scrollbarRect.SetWidth(scrollbarRect.GetWidth() - m_ScrollbarWidth - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2));
        }

        return scrollbarRect;
    }

    const Nexus::Graphics::Rectangle<float> Scrollable::GetHorizontalScrollbarBackgroundArea() const
    {
        const Nexus::Graphics::Rectangle<float> contentRegion = GetContentRegionAvailable();

        Nexus::Graphics::Rectangle<float> scrollbarFillRect = Nexus::Graphics::Rectangle<float>(contentRegion.GetLeft(),
                                                                                                contentRegion.GetBottom() - m_ScrollbarWidth - (m_ScrollbarBorderThickness * 2) - (m_ScrollbarPadding * 2),
                                                                                                contentRegion.GetWidth(),
                                                                                                m_ScrollbarWidth + (m_ScrollbarBorderThickness * 2) + (m_ScrollbarPadding * 2));
        return scrollbarFillRect;
    }

    const bool Scrollable::RequiresVerticalScrollbar() const
    {
        const auto &contentSize = GetScrollableAreaInControlSpace();
        const auto &contentRegionAvailable = GetContentRegionAvailableTranslated();
        if (contentSize.GetHeight() > contentRegionAvailable.GetRight() || contentSize.GetTop() < contentRegionAvailable.GetTop())
        {
            return true;
        }
        return false;
    }

    const bool Scrollable::RequiresHorizontalScrollbar() const
    {
        const auto &contentSize = GetScrollableAreaInControlSpace();
        const auto &contentRegionAvailable = GetContentRegionAvailableTranslated();
        if (contentSize.GetRight() > contentRegionAvailable.GetRight() || contentSize.GetLeft() < contentRegionAvailable.GetLeft())
        {
            return true;
        }

        return false;
    }
}