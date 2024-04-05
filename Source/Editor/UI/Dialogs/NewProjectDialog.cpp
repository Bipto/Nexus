#include "NewProjectDialog.hpp"

#include "imgui.h"

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
            }

            ImGui::End();
        }
    }

}
