#pragma once

#include "Nexus/Application.hpp"
#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

namespace Editor
{
    class Layout
    {
    public:
        Layout(Nexus::Application *app);
        void Render(Nexus::Time time);

    private:
        std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
    };
}