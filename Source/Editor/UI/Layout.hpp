#pragma once

#include "Nexus/Application.hpp"
#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Panel.hpp"

#include <map>

namespace Editor
{
    class Layout
    {
    public:
        Layout(Nexus::Application *app);
        void Render(Nexus::Time time);

    private:
        void RenderViewport();
        void ApplyDarkTheme();
        void RenderMainMenubar();
        void OpenProject();

    private:
        std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
        Nexus::Application *m_Application = nullptr;

        std::map<std::string, std::unique_ptr<Panel>> m_Panels;
    };
}