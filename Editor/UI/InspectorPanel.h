#include "Panel.h"

class InspectorPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Inspector");

            if (m_SelectedEntityID != -1)
            {
                auto& entity = m_Project->GetActiveScene()->GetEntities()[m_SelectedEntityID];
                //ImGui::Text(m_SelectedEntity->GetName().c_str());
                //ImGui::Text(entity.GetName().c_str());

                auto name = entity.GetName();
                if (ImGui::InputText("Name", &name))
                    entity.SetName(name);

                for (auto component : entity.GetComponents())
                {
                    ImGui::Separator();
                    ImGui::Text(component->GetName());
                    component->RenderUI();
                }
            }

            ImGui::End();
        }
};