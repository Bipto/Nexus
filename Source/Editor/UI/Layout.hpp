#pragma once

#include "Nexus/Application.hpp"
#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Panel.hpp"

#include "Nexus/nxpch.hpp"

namespace Editor
{
    class Layout
    {
    public:
        Layout(Nexus::Application *app);
        void Render(Nexus::Time time);
        void LoadProject(const std::string &path);

        void SaveLayout(const std::string &path) const;
        bool LoadLayout(const std::string &path);

    private:
        void OpenProject();
        void RenderViewport();
        void ApplyDarkTheme();
        void RenderMainMenubar();

    private:
        std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
        Nexus::Application *m_Application = nullptr;

        std::map<std::string, std::unique_ptr<Panel>> m_Panels;
        std::map<std::string, std::unique_ptr<Panel>> m_Dialogs;
    };
}