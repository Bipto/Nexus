#include "Panel.h"
#include "NexusEngine.h"

class NewScenePanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            static std::string name{};

            ImGui::Begin("New Scene", &m_Enabled);
            ImGui::Text("Name: ");
            ImGui::SameLine();
            ImGui::InputText(" ", &name);

            if (ImGui::Button("Create"))
            {
                std::string sceneName = name;
                m_Project->AddScene(sceneName);

                name.clear();
            }

            ImGui::End();
        }
};