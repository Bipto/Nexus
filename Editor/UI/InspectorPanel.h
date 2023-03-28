#include "Panel.h"

class InspectorPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Inspector");

            Nexus::Component* componentToRemove = nullptr;

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

                //defer removal of component until UI rendering has been completed
                if (componentToRemove)
                    entity.RemoveComponent(componentToRemove);
            }

            ImGui::End();
        }
};