#pragma once

#include "Panel.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"

#include "TestComp.hpp"

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

			if (ImGui::BeginPopupContextItem("Add Component"))
			{
				const std::map<const char *, Nexus::ECS::CreateComponentFunc> &availableComponents = Nexus::ECS::GetAvailableComponents();

				ImGui::Text("Add Components");
				ImGui::Separator();

				for (const auto &[name, creationFunction] : availableComponents)
				{
					const char *displayName = Nexus::ECS::GetDisplayNameFromTypeName(name);
					if (ImGui::Selectable(displayName))
					{
						creationFunction(scene->Registry, *entity);
					}
				}

				ImGui::EndPopup();
			}

			std::stringstream ss;
			ss << "ID: " << entity->ID.Value;
			ImGui::Text(ss.str().c_str());
			ImGui::InputText("Name", &entity->Name);

			std::vector<Nexus::ECS::ComponentPtr> components = scene->Registry.GetAllComponents(entity->ID);
			for (Nexus::ECS::ComponentPtr component : components)
			{
				const char *displayName = Nexus::ECS::GetDisplayNameFromTypeName(component.typeName);
				ImGui::Text(displayName);

				ImGui::PushID(component.data);
				Nexus::ECS::RenderComponent(component);
				ImGui::PopID();

				ImGui::Separator();
			}

			ImGui::Button("+");
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			{
				ImGui::OpenPopup("Add Component");
			}
		}

		ImGui::End();
	}
};