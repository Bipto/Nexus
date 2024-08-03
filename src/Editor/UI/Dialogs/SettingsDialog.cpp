#include "SettingsDialog.hpp"

namespace Editor
{
    SettingsDialog::SettingsDialog(std::map<std::string, std::unique_ptr<Panel>> *panels)
        : m_Panels(panels)
    {
    }

    void SettingsDialog::OnLoad()
    {
    }

    void SettingsDialog::OnRender()
    {
        if (m_Enabled)
        {
            if (ImGui::Begin(SETTINGS_DIALOG_NAME, &m_Enabled))
            {
                for (auto it = m_Panels->begin(); it != m_Panels->end(); it++)
                {
                    std::unique_ptr<Panel> &panel = it->second;

                    bool enabled = panel->IsEnabled();

                    if (ImGui::Checkbox(it->first.c_str(), &enabled))
                    {
                        if (enabled)
                        {
                            panel->Enable();
                        }
                        else
                        {
                            panel->Disable();
                        }
                    }
                }
            }

            ImGui::End();
        }
    }
}