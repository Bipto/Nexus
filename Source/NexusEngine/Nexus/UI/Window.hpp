#pragma once

#include "Control.hpp"
#include "Scrollable.hpp"

#include <string>

namespace Nexus::UI
{
    class Window : public Control
    {
    public:
        Window();
        virtual ~Window();
        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;
        virtual void Update() override;

        void SetDraggable(bool draggable);
        const bool IsDraggable() const;

        void SetCloseable(bool closeable);
        const bool IsCloseable() const;

        void SetTitlebarBackgroundColour(const glm::vec4 &colour);
        const glm::vec4 &GetTitlebarBackgroundColour() const;

        void AddControl(Control *control);
        void RemoveControl(Control *control);

        virtual void SetCanvas(Canvas *canvas) override;

        Scrollable *GetScrollable() const;

    private:
        const Nexus::Graphics::Rectangle<float> GetTitlebarRectangle() const;
        const Nexus::Graphics::Rectangle<float> GetAvailableContentRegion() const;

    private:
        std::string m_Title = "Window";
        bool m_Draggable = true;
        bool m_Closeable = true;
        bool m_Dragging = false;
        glm::vec4 m_TitlebarBackgroundColour = {0.35f, 0.35f, 0.35f, 1.0f};

        Scrollable *m_Scrollable = nullptr;
        bool m_ResizeX = false;
        bool m_ResizeY = false;
    };
}