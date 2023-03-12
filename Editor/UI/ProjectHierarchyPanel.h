#include "Panel.h"

class ProjectHierarchyPanel : public Panel
{
    public:
        void LoadProject(Nexus::Ref<Nexus::Project> project)
        {
            m_Project = project;
        }

        void UnloadProject()
        {
            m_Project = {};
        }

        virtual void OnRender() override
        {
            ImGui::Begin("Project Hierarchy");

            if (m_Project)
            {
                for (auto scene : m_Project->GetScenes())
                {
                    auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;
                    auto text = scene->GetName();

                    if (ImGui::TreeNodeEx(text.c_str(), flags))
                    {
                        ImGui::TreePop();
                    }
                }
            }

            ImGui::End();
        }
    private:
        Nexus::Ref<Nexus::Project> m_Project;
};