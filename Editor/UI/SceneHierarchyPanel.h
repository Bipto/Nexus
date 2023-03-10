#include "Panel.h"

class SceneHierarchyPanel : public Panel
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
            ImGui::Begin("Scene Hierachy");

            if (m_Project)
            {             
                for (auto entity : m_Project->GetActiveScene()->GetEntities())
                {
                    auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;
                    auto text = entity.GetName();

                    if (ImGui::TreeNodeEx(text.c_str(), flags))
                    {
                        ImGui::TreePop();
                    }
                }
                
                if (ImGui::BeginPopupContextWindow("Popup"))
                {
                    if (ImGui::Selectable("New Entity"))
                    {
                        m_Project->GetActiveScene()->AddEmptyEntity();
                    }
                    ImGui::EndPopup();
                }
            }
            
            ImGui::End();
        }
    private:
        Nexus::Ref<Nexus::Project> m_Project;
};