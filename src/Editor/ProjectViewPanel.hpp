#pragma once

#include "Panel.hpp"

class ProjectViewPanel : public Panel
{
  public:
	ProjectViewPanel() : Panel("Project")
	{
	}

	void Render() final
	{
		ImGui::Begin(m_Name.c_str(), &m_Open);

		if (m_Project)
		{
			for (const Nexus::SceneInfo &scene : m_Project->GetScenes()) { ImGui::Text(scene.Name.c_str()); }
		}

		ImGui::End();
	}
};