#include "Panel.h"

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

                    m_ActiveProject = new Nexus::Project(name);
                    m_ActiveProject->Serialize(m_ProjectFilePath);

                    #endif
                }

                ImGui::End();
            #endif
        }
    
    private:
        Nexus::Project* m_ActiveProject = nullptr;
        bool m_NewProjectPanelVisible = false;
        std::string m_ProjectFilePath;
};