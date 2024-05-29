#pragma once

#include "Control.hpp"

#include <vector>

namespace Nexus::UI
{
    class Scrollable : public Control
    {
    public:
        virtual ~Scrollable();

        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;
        virtual void Update() override;
        void AddControl(Control *control);
        void RemoveControl(Control *control);

        void SetScrollbarWidth(float width);
        const float GetScrollbarWidth() const;

        void SetScrollbarBackgroundColour(const glm::vec4 &colour);
        const glm::vec4 &GetScrollbarBackgroundColour() const;

        void SetScrollbarForegroundColour(const glm::vec4 &colour);
        const glm::vec4 &GetScrollbarForegroundColour() const;

        void SetScrollbarBorderThickness(const float thickness);
        const float GetScrollbarBorderThickness() const;

        void SetScrollbarPadding(const float padding);
        const float GetScrollbarPadding() const;

        void SetScrollSpeed(const float speed);
        const float GetScrollSpeed() const;

    protected:
        void SetAvailableScrollableArea(const Nexus::Graphics::Rectangle<float> &rect);
        const Nexus::Graphics::Rectangle<float> GetAvailableScrollableArea() const;

    private:
        const Nexus::Graphics::Rectangle<float> GetScrollLimits() const;
        const Nexus::Graphics::Rectangle<float> GetScrollableAreaInControlSpace() const;
        const Nexus::Graphics::Rectangle<float> GetVerticalScrollbarControlArea() const;
        const Nexus::Graphics::Rectangle<float> GetVerticalScrollbarBackgroundArea() const;
        const Nexus::Graphics::Rectangle<float> GetHorizontalScrollbarControlArea() const;
        const Nexus::Graphics::Rectangle<float> GetHorizontalScrollbarBackgroundArea() const;

        const bool RequiresVerticalScrollbar() const;
        const bool RequiresHorizontalScrollbar() const;
        void HandleVerticalScroll();
        void HandleHorizontalScroll();

    private:
        std::vector<Control *> m_Controls;
        float m_ScrollbarWidth = 15.0f;
        bool m_ScrollbarYGrabbed = false;
        bool m_ScrollbarXGrabbed = false;
        glm::vec4 m_ScrollbarBackground = {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 m_ScrollbarForeground = {0.55f, 0.55f, 0.55f, 1.0f};
        float m_ScrollbarBorderThickness = 1.0f;
        float m_ScrollbarPadding = 1.0f;
        float m_ScrollSpeed = 25.0f;
        Nexus::Graphics::Rectangle<float> m_AvailableScrollableArea;
    };
}