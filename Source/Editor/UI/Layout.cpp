#include "Layout.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"
#include "Nexus/FileSystem/FileDialogs.hpp"

#include "Dialogs/NewProjectDialog.hpp"

#include "SceneViewport.hpp"
#include "SceneHierarchy.hpp"

#include "Nexus/Runtime/Project.hpp"

namespace Editor
{
    Layout::Layout(Nexus::Application *app)
        : m_Application(app)
    {
        m_ImGuiRenderer = std::make_unique<Nexus::ImGuiUtils::ImGuiGraphicsRenderer>(app);

        ImGuiIO &io = ImGui::GetIO();

        std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/fonts/roboto/roboto-regular.ttf");
        const uint32_t size = 28;
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        m_ImGuiRenderer->RebuildFontAtlas();

        ApplyDarkTheme();

        m_Dialogs[NEW_PROJECT_DIALOG_NAME] = std::make_unique<NewProjectDialog>();
        m_Panels[SCENE_VIEWPORT_NAME] = std::make_unique<SceneViewport>(app->GetGraphicsDevice(), m_ImGuiRenderer.get());
        m_Panels[SCENE_HIERARCHY_NAME] = std::make_unique<SceneHierarchy>();
    }

    void Layout::Render(Nexus::Time time)
    {
        m_ImGuiRenderer->BeforeLayout(time);
        RenderViewport();
        m_ImGuiRenderer->AfterLayout();
    }

    void Layout::LoadProject(const std::string &path)
    {
        Nexus::Ref<Nexus::Project> project = Nexus::Project::Deserialize(path);
        Nexus::Project::s_ActiveProject = project;
    }

    void Layout::SaveLayout(const std::string &path) const
    {
        ImGui::SaveIniSettingsToDisk(path.c_str());
    }

    bool Layout::LoadLayout(const std::string &path)
    {
        if (std::filesystem::exists(path))
        {
            ImGui::LoadIniSettingsFromDisk(path.c_str());
            return true;
        }

        return false;
    }

    void Layout::RenderViewport()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

        if (ImGui::Begin("Dockspace", nullptr, flags))
        {
            RenderMainMenubar();
            ImGui::PopStyleVar(3);
            ImGui::DockSpace(ImGui::GetID("Dockspace"));

            for (const auto &panel : m_Panels)
            {
                panel.second->OnRender();
            }

            for (const auto &dialog : m_Dialogs)
            {
                dialog.second->OnRender();
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
                    std::unique_ptr<Panel> &panel = m_Dialogs.at(NEW_PROJECT_DIALOG_NAME);
                    panel->Enable();
                }

                if (ImGui::MenuItem("New Scene"))
                {
                }

                ImGui::EndMenu();
            }

            if (Nexus::Project::s_ActiveProject)
            {
                if (ImGui::MenuItem("Save"))
                {
                    Nexus::Project::s_ActiveProject->Serialize();
                }
            }

            if (ImGui::MenuItem("Load Project"))
            {
                OpenProject();
            }

            if (ImGui::MenuItem("Quit"))
            {
                m_Application->Close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows", true))
        {
            const bool leftMousePressed = ImGui::IsMouseClicked(0);
            for (auto it = m_Panels.begin(); it != m_Panels.end(); it++)
            {
                std::unique_ptr<Panel> &panel = it->second;

                bool enabled = panel->IsEnabled();

                if (ImGui::Checkbox(it->first.c_str(), &enabled))
                {
                    if (enabled)
                    {
                        panel->Enable();
                    }
                    else
                    {
                        panel->Disable();
                    }
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    void Layout::OpenProject()
    {
        std::vector<const char *> filters = {"*.proj"};
        auto path = Nexus::FileDialogs::OpenFile(filters);

        // check that the user didn't cancel the dialog
        if (path)
        {
            LoadProject(path);
        }
    }
}