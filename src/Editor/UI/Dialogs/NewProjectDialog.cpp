#include "NewProjectDialog.hpp"
#include "Nexus-Core/FileSystem/FileDialogs.hpp"

#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/Runtime/Project.hpp"

#include "Editor.hpp"

#include "Nexus-Core/nxpch.hpp"

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
                        std::string directory = m_Path + std::string("\\") + m_Name;
                        std::filesystem::path fullpath = directory + std::string("\\") + m_Name + extension;

                        auto project = Nexus::CreateRef<Nexus::Project>(m_Name, directory);
                        project->Serialize();

                        Nexus::Project::s_ActiveProject = project;

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
