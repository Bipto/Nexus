#pragma once

#include "Nexus/Renderer/BatchRenderer.hpp"

#include "Nexus/Point.hpp"

#include "Nexus/Graphics/Font.hpp"

#include <glm/glm.hpp>

namespace Nexus::UI
{
    class Control
    {
    public:
        virtual void Update() = 0;
        virtual void Render(Graphics::BatchRenderer *batchRenderer) = 0;

        void SetPosition(const Point<int> &position);
        void SetSize(const Point<int> &size);
        void SetBackgroundColour(const glm::vec4 &colour);
        void SetFont(Graphics::Font *font);

        const Point<int> &GetPosition() const;
        const Point<int> &GetSize() const;
        const glm::vec4 &GetBackgroundColour() const;
        const Graphics::Font *GetFont() const;

    protected:
        Point<int> m_Position = {0, 0};
        Point<int> m_Size = {0, 0};
        glm::vec4 m_BackgroundColour = {1, 1, 1, 1};
        glm::vec4 m_ForegroundColour = {0, 0, 0, 1};
        Graphics::Font *m_Font = nullptr;
    };
}