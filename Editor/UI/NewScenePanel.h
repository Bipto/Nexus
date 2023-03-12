#include "Panel.h"
#include "NexusEngine.h"

class NewScenePanel : public Panel
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
    private:
        Nexus::Ref<Nexus::Project> m_Project;
};