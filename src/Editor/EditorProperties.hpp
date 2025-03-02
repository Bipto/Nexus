#pragma once

#include "Panel.hpp"

class EditorPropertiesPanel : public Panel
{
  public:
	EditorPropertiesPanel(std::vector<Panel *> *panels) : Panel("Editor Properties")
	{
		m_Panels = panels;
	}

	void Render() final
	{
		ImGui::Begin(m_Name.c_str(), &m_Open);

		for (auto panel : *m_Panels)
		{
			bool panelOpen = panel->IsOpen();

			if (panel->GetName() == m_Name)
			{
				continue;
			}

			if (ImGui::Checkbox(panel->GetName().c_str(), &panelOpen))
			{
				if (panelOpen)
				{
					panel->Open();
				}
				else
				{
					panel->Close();
				}
			}
		}

		ImGui::End();
	}

  private:
	std::vector<Panel *> *m_Panels = nullptr;
};