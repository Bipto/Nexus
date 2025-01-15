#pragma once

#include "Panel.hpp"

class InspectorPanel : public Panel
{
  public:
	InspectorPanel() : Panel("Inspector")
	{
	}

	void Render() final
	{
		bool renderContent = m_Project && m_Project->IsSceneLoaded() && m_SelectedEntity.has_value();

		ImGui::Begin(m_Name.c_str(), &m_Open);

		if (renderContent)
		{
			Nexus::Scene  *scene  = m_Project->GetLoadedScene();
			Nexus::Entity *entity = scene->GetEntity(m_SelectedEntity.value());

			std::stringstream ss;
			ss << "ID: " << entity->ID.Value;
			ImGui::Text(ss.str().c_str());
			ImGui::InputText("Name", &entity->Name);
		}

		ImGui::End();
	}
};