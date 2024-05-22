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

    private:
        const Nexus::Graphics::Rectangle<float> GetScrollLimits() const;
        const Nexus::Graphics::Rectangle<float> GetScrollableArea() const;
        const Nexus::Graphics::Rectangle<float> GetScrollbar() const;
        void HandleScroll();

    private:
        std::vector<Control *> m_Controls;
        float m_ScrollbarWidth = 15.0f;
    };
}