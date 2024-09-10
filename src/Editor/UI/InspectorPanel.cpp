#include "InspectorPanel.hpp"

#include "Layout.hpp"

namespace Editor
{
	void Inspector::OnLoad()
	{
	}

	void Inspector::OnRender()
	{
		if (m_Enabled)
		{
			if (ImGui::Begin(INSPECTOR_PANEL_NAME, &m_Enabled))
			{
				Nexus::Entity *entity = Layout::s_SelectedEntity;

				if (entity)
				{
					ImGui::Text(entity->GetName().c_str());
					ImGui::Text("%u", entity->GetID());
				}
			}

			ImGui::End();
		}
	}

}	 // namespace Editor