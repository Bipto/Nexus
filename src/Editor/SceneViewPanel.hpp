#pragma once

#include "Panel.hpp"

class SceneViewPanel : public Panel
{
  public:
	SceneViewPanel() : Panel("Scene")
	{
	}

	void Render() final
	{
		if (!m_Open)
		{
			return;
		}

		ImGui::Begin(m_Name.c_str());

		if (m_Project && m_Project->IsSceneLoaded())
		{
			Nexus::Scene *scene = m_Project->GetLoadedScene();
			for (const auto &entity : scene->GetEntities()) { ImGui::Text(entity.GetName().c_str()); }
		}

		ImGui::End();
	}
};