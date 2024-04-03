#include "Layout.hpp"

namespace Editor
{
    Layout::Layout(Nexus::Application *app)
    {
        m_ImGuiRenderer = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(app);
        ImGui::GetIO().Fonts->AddFontDefault();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        m_ImGuiRenderer->RebuildFontAtlas();
    }

    void Layout::Render(Nexus::Time time)
    {
        m_ImGuiRenderer->BeforeLayout(time);
        ImGui::ShowDemoWindow();
        m_ImGuiRenderer->AfterLayout();
    }
}
