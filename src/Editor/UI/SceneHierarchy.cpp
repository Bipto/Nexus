#include "SceneHierarchy.hpp"

#include "Layout.hpp"
#include "Nexus-Core/Runtime/Project.hpp"

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

    if (ImGui::CollapsingHeader("Scene Properties", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
    {
        glm::vec4 clearColour = scene->GetClearColour();

        if (ImGui::ColorEdit4("Clear Colour", glm::value_ptr(clearColour)))
        {
            scene->SetClearColour(clearColour);
        }

        std::string name = scene->GetName();

        if (ImGui::InputText("Name", &name))
        {
            scene->SetName(name);
        }
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Entities", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto &entity : scene->GetEntities())
        {
            ImGui::PushID(entity.GetID());
            if (ImGui::Selectable(entity.GetName().c_str()))
            {
                Layout::s_SelectedEntity = &entity;
            }
            ImGui::PopID();
        }

        if (ImGui::Button("+"))
        {
            scene->AddEmptyEntity();
        }
    }
}
} // namespace Editor