#include "Panel.h"

class SceneHierarchyPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Scene Hierachy");

            if (m_Project)
            {             
                auto activeScene = m_Project->GetActiveScene();

                for (int i = 0; i < activeScene->GetEntities().size(); i++)
                {
                    auto& entity = activeScene->GetEntities()[i];

                    auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;
                    auto entityName = entity.GetName().c_str();

                    if (ImGui::TreeNodeEx(entityName, flags))
                    {
                        if (ImGui::IsItemClicked())
                        {
                            m_EventHandler.Invoke(i);
                        }

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

        void BindEntitySelectedFunction(Delegate<int> delegate)
        {
            m_EventHandler.Bind(delegate);
        }   

        void UnbindEntitySelectedFunction(Delegate<int> delegate)
        {
            m_EventHandler.Unbind(delegate);
        }

    private:
        Nexus::EventHandler<int> m_EventHandler;
};