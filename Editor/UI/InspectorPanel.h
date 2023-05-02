#include "Panel.h"

class InspectorPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Inspector");

            Nexus::Component* componentToRemove = nullptr;

            //render selected entity info
            if (m_SelectedEntityID != -1)
            {
                auto& entity = m_Project->GetActiveScene()->GetEntities()[m_SelectedEntityID];

                auto name = entity.GetName();
                if (ImGui::InputText("Name", &name))
                    entity.SetName(name);
                ImGui::Separator();

                int index = 0;
                for (auto component : entity.GetComponents())
                {
                    ImGui::PushID(index);
                    ImGui::Text(component->GetName());
                    component->RenderUI();
                    if (ImGui::Button("Remove Component"))
                    {
                        componentToRemove = component;
                    }

                    ImGui::Separator();
                    ImGui::PopID();
                    index++;
                }

                if (ImGui::Button("+"))
                {
                    ImGui::OpenPopup("new_component");
                }

                if (ImGui::BeginPopup("new_component"))
                {
                    auto& registry = Nexus::GetComponentRegistry();
                    for (auto component : registry.GetComponents())
                    {
                        if (ImGui::Selectable(component.first.c_str()))
                        {
                            auto newComponent = component.second->Clone();
                            entity.AddComponent(newComponent);
                        }
                    }


                    ImGui::EndPopup();
                }

                //defer removal of component until UI rendering has been completed
                if (componentToRemove)
                    entity.RemoveComponent(componentToRemove);
            }

            //render scene info
            else
            {
                if (m_Project)
                {
                    auto scene = m_Project->GetActiveScene();
                    auto& sceneName = scene->GetName();
                    auto& clearColor = scene->GetClearColor();

                    if (scene)
                    {
                        ImGui::InputText("Name", &sceneName);
                        ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearColor));
                    }
                }
            }

            ImGui::End();
        }
};