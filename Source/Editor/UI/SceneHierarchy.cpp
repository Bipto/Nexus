#include "SceneHierarchy.hpp"

#include "Nexus/Runtime/Project.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace Editor
{
    void SceneHierarchy::OnLoad()
    {
    }

    void SceneHierarchy::OnRender()
    {
        if (m_Enabled)
        {
            if (ImGui::Begin(SCENE_HIERARCHY_NAME, &m_Enabled))
            {
                RenderControls();
            }

            ImGui::End();
        }
    }

    void SceneHierarchy::RenderControls()
    {
        Nexus::Ref<Nexus::Project> project = Nexus::Project::s_ActiveProject;

        if (!project)
        {
            return;
        }

        Nexus::Scene *scene = project->GetLoadedScene();

        if (!scene)
        {
            return;
        }

        glm::vec4 clearColour = scene->GetClearColour();

        if (ImGui::ColorPicker4("Clear Colour", glm::value_ptr(clearColour)))
        {
            scene->SetClearColour(clearColour);
        }

        ImGui::Separator();

        if (ImGui::BeginPopupContextItem("PopupMenu", ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("New Entity"))
            {
                scene->AddEmptyEntity();
            }

            ImGui::EndPopup();
        }

        for (const auto &entity : scene->GetEntities())
        {
            ImGui::Text(entity.GetName().c_str());
        }

        /* if (ImGui::Button("New Entity"))
        {
            scene->AddEmptyEntity();
        } */
    }
}