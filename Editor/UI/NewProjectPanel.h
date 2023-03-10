#include "Panel.h"
#include "NexusEngine.h"

#include <functional>

class NewProjectPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            #ifndef __EMSCRIPTEN__
                static std::string name{};
                static std::string path{};

                ImGui::Begin("New Project", &m_Enabled);

                //name
                ImGui::Text("Name: ");
                ImGui::SameLine();
                ImGui::InputText(" ", &name);

                //directory
                ImGui::Text("Directory: ");
                ImGui::SameLine();
                ImGui::Text(path.c_str());

                if (ImGui::Button("Choose folder..."))
                {
                    #ifndef __EMSCRIPTEN__

                    wchar_t* p = tinyfd_selectFolderDialogW(
                        L"Select a folder",
                        L"C:\\"  
                    );

                    if (p)
                    {
                        std::wstring ws(p);
                        path = {ws.begin(), ws.end()};
                        m_ProjectFilePath = {path};
                    }

                    #endif

                }

                if (ImGui::Button("Create"))
                {
                    #ifndef __EMSCRIPTEN__

                    std::filesystem::path path{m_ProjectFilePath};
                    std::string extension(".proj");
                    path /= name + std::string("\\") + name + extension;
                    std::filesystem::create_directories(path.parent_path());

                    auto project = Nexus::CreateRef<Nexus::Project>(name);
                    project->Serialize(m_ProjectFilePath);
                    m_ProjectCreatedEventHandler.Invoke(project);

                    #endif
                }

                ImGui::End();
            #endif
        }

        void Subscribe(Delegate<Nexus::Ref<Nexus::Project>> function)
        {
            m_ProjectCreatedEventHandler.Bind(function);
        }

        void Unsubscribe(Delegate<Nexus::Ref<Nexus::Project>> function)
        {
            m_ProjectCreatedEventHandler.Unbind(function);
        }
    
    private:
        bool m_NewProjectPanelVisible = false;
        std::string m_ProjectFilePath;
        Nexus::EventHandler<Nexus::Ref<Nexus::Project>> m_ProjectCreatedEventHandler;
        
};