#include "Panel.h"

class SceneHierarchyPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Scene Hierachy");
            bool itemHovered = false;
            int indexToRemove = -1;

            if (m_Project)
            {             
                auto activeScene = m_Project->GetActiveScene();
                auto sceneName = activeScene->GetName().c_str();
                auto& entities = activeScene->GetEntities();

                auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

                if (ImGui::TreeNodeEx(sceneName, flags))
                {
                    for (int i = 0; i < entities.size(); i++)
                    {
                        auto& entity = entities[i];
                        auto entityName = entity.GetName().c_str();

                        if (ImGui::TreeNodeEx(entityName, flags))
                        {
                            if (ImGui::IsItemClicked())
                            {
                                m_EventHandler.Invoke(i);
                            }

                            if (ImGui::IsItemHovered())
                            {
                                itemHovered = true;
                            }

                            ImGui::PushID(std::to_string(i).c_str());
                            if (ImGui::BeginPopupContextItem(entityName))
                            {
                                if (ImGui::BeginMenu("New Component"))
                                {
                                    auto& registry = Nexus::GetComponentRegistry();
                                    for (auto component : registry.GetComponents())
                                    {                                        
                                        ImGui::Selectable(component.first.c_str());
                                    }

                                    ImGui::EndMenu();
                                }     

                                if (ImGui::MenuItem("Delete Entity"))
                                {
                                    indexToRemove = i;
                                }

                                ImGui::EndPopup();
                            }
                            ImGui::PopID();

                            ImGui::TreePop();
                        }
                    }   

                    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1) && !itemHovered)
                    {
                        ImGui::OpenPopup("create_entity");
                    }

                    if (ImGui::BeginPopup("create_entity"))
                    {
                        if (ImGui::Selectable("New Entity"))
                            activeScene->AddEmptyEntity();
                        ImGui::EndPopup();
                    }

                    ImGui::TreePop();
                }

                if (indexToRemove != -1)
                {
                    auto iterator = entities.begin() + indexToRemove;

                    if (iterator < entities.end())
                    {
                        m_EventHandler.Invoke(-1);
                        entities.erase(iterator);
                    }
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