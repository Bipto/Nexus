#include "NewProjectDialog.hpp"
#include "Nexus/FileSystem/FileDialogs.hpp"
#include <string>

#include "Nexus/Types.hpp"
#include "Nexus/Runtime/Project.hpp"

namespace Editor
{
    void NewProjectDialog::OnLoad()
    {
    }

    void NewProjectDialog::OnRender()
    {
        if (m_Enabled)
        {
            if (ImGui::Begin(NEW_PROJECT_DIALOG_NAME, &m_Enabled))
            {
                static std::string name;

                // project name
                ImGui::Text("Name: ");
                ImGui::SameLine();
                ImGui::InputText(" ", &name);

                // directory
                ImGui::Text("Directory: ");
                ImGui::SameLine();
                ImGui::Text(m_Path.c_str());

                for (const auto &c : ImGui::GetIO().InputQueueCharacters)
                {
                    std::cout << c << std::endl;
                }

                if (ImGui::Button("Choose folder..."))
                {
                    auto p = Nexus::FileDialogs::OpenFolder(
                        "Select a folder",
                        "C:\\");

                    if (p)
                    {
                        m_Path = std::string(p);
                    }
                }

                if (!m_Path.empty())
                {
                    if (ImGui::Button("Create project"))
                    {
                        std::string extension = ".proj";
                        std::filesystem::path fullpath = m_Path + std::string("\\") + m_Name + std::string("\\") + m_Name + extension;
                        std::filesystem::create_directories(fullpath.parent_path());

                        auto project = Nexus::CreateRef<Nexus::Project>(m_Name, fullpath.string());
                        project->Serialize(fullpath.string());

                        m_Name = {};
                        m_Path = {};
                        Disable();
                    }
                }
            }

            ImGui::End();
        }
    }

}
