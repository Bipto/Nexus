#pragma once

#include "Nexus/Renderer/BatchRenderer.hpp"
#include "Nexus/Point.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Nexus::UI
{
    class Control;

    class Canvas
    {
    public:
        Canvas(Graphics::GraphicsDevice *device, Graphics::Swapchain *swapchain);

        void SetPosition(const Point<uint32_t> &position);
        void SetSize(const Point<uint32_t> &size);
        void SetBackgroundColour(const glm::vec4 &color);

        const Point<uint32_t> &GetPosition() const;
        const Point<uint32_t> &GetSize() const;
        const glm::vec4 &GetBackgroundColour() const;

        void Render() const;

        void AddControl(Control *control);
        void RemoveControl(Control *control);

    private:
        Point<uint32_t> m_Position = {0, 0};
        Point<uint32_t> m_Size = {0, 0};

        glm::vec4 m_BackgroundColour = {0, 0, 0, 0};

        std::unique_ptr<Nexus::Graphics::BatchRenderer> m_BatchRenderer = nullptr;

        std::vector<Control *> m_Controls;
    };
}