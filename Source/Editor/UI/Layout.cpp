#include "Layout.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "Dialogs/NewProjectDialog.hpp"

namespace Editor
{
    Layout::Layout(Nexus::Application *app)
        : m_Application(app)
    {
        m_ImGuiRenderer = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(app);

        ImGuiIO &io = ImGui::GetIO();

        std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/fonts/roboto/roboto-regular.ttf");
        const uint32_t size = 22;
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        m_ImGuiRenderer->RebuildFontAtlas();

        ApplyDarkTheme();

        m_Panels[NEW_PROJECT_DIALOG_NAME] = std::make_unique<NewProjectDialog>();
    }

    void Layout::Render(Nexus::Time time)
    {
        m_ImGuiRenderer->BeforeLayout(time);
        RenderViewport();
        m_ImGuiRenderer->AfterLayout();
    }

    void Layout::RenderViewport()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_MenuBar;

        if (ImGui::Begin("Dockspace", nullptr, flags))
        {
            RenderMainMenubar();
            ImGui::PopStyleVar(3);
            ImGui::DockSpace(ImGui::GetID("Dockspace"));

            for (const auto &panel : m_Panels)
            {
                panel.second->OnRender();
            }
        }
        ImGui::End();
    }

    void Layout::ApplyDarkTheme()
    {
        auto &colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = {0.05f, 0.055f, 0.06f, 1.0f};

        colors[ImGuiCol_Header] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_HeaderHovered] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_HeaderActive] = {0.3f, 0.3f, 0.31f, 1.0f};

        colors[ImGuiCol_Button] = {0.15f, 0.15f, 0.15f, 1.0f};
        colors[ImGuiCol_ButtonHovered] = {0.25f, 0.25f, 0.25f, 1.0f};
        colors[ImGuiCol_ButtonActive] = {0.1f, 0.1f, 0.1f, 1.0f};

        colors[ImGuiCol_FrameBg] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_FrameBgHovered] = {0.4f, 0.4f, 0.41f, 1.0f};
        colors[ImGuiCol_FrameBgActive] = {0.3f, 0.3f, 0.31f, 1.0f};

        colors[ImGuiCol_Tab] = {0.15f, 0.15f, 0.15f, 1.0f};
        colors[ImGuiCol_TabHovered] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_TabActive] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_TabUnfocused] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = {0.15f, 0.15f, 0.16f, 1.0f};

        colors[ImGuiCol_MenuBarBg] = {0.08f, 0.085f, 0.09f, 1.0f};
        colors[ImGuiCol_TitleBg] = {0.05f, 0.055f, 0.05f, 1.0f};
        colors[ImGuiCol_TitleBgActive] = {0.08f, 0.085f, 0.09f, 1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = {0.05f, 0.055f, 0.06f, 1.0f};

        colors[ImGuiCol_CheckMark] = {0.55f, 0.55f, 0.55f, 1.0f};
        colors[ImGuiCol_Text] = {0.85f, 0.85f, 0.85f, 1.0f};
        colors[ImGuiCol_SliderGrab] = {0.55f, 0.55f, 0.55f, 1.0f};
        colors[ImGuiCol_SliderGrabActive] = {0.55f, 0.55f, 0.55f, 1.0f};
    }

    void Layout::RenderMainMenubar()
    {
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File", true))
        {
            if (ImGui::BeginMenu("New...", true))
            {
                if (ImGui::MenuItem("New Project"))
                {
                }

                if (ImGui::MenuItem("New Scene"))
                {
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Quit"))
            {
                m_Application->Close();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}