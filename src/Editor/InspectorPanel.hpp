#pragma once

#include "Panel.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"

struct ComponentToRemove
{
	Nexus::GUID EntityID	   = {};
	const char *TypeName	   = {};
	size_t		ComponentIndex = {};
};

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
				const std::map<std::string, Nexus::ECS::CreateComponentFunc> &availableComponents = Nexus::ECS::GetAvailableComponents();

				ImGui::Text("Add Components");
				ImGui::Separator();

				for (const auto &[name, creationFunction] : availableComponents)
				{
					std::string displayName = Nexus::ECS::GetDisplayNameFromTypeName(name);
					if (ImGui::Selectable(displayName.c_str()))
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
			ImGui::Separator();

			std::vector<Nexus::ECS::ComponentPtr> components = scene->Registry.GetAllComponents(entity->ID);
			for (Nexus::ECS::ComponentPtr component : components)
			{
				ImGui::PushID(component.data);
				std::string displayName = Nexus::ECS::GetDisplayNameFromTypeName(component.typeName);
				ImGui::Text(displayName.c_str());

				Nexus::ECS::RenderComponent(component);

				if (ImGui::Button("Remove"))
				{
					ComponentToRemove componentToRemove {.EntityID		 = entity->ID,
														 .TypeName		 = component.typeName,
														 .ComponentIndex = component.componentIndex};
					m_ComponentsToRemove.push_back(componentToRemove);
				}
				ImGui::Separator();
				ImGui::PopID();
			}

			ImGui::Button("+");
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			{
				ImGui::OpenPopup("Add Component");
			}
		}

		ImGui::End();
	}

	void Update() final
	{
		if (!m_Project && !m_Project->IsSceneLoaded() && m_ComponentsToRemove.size() > 0)
		{
			return;
		}

		Nexus::Scene *scene = m_Project->GetLoadedScene();

		for (const auto &component : m_ComponentsToRemove)
		{
			int x = 0;
			scene->Registry.RemoveComponent(component.EntityID, component.TypeName, component.ComponentIndex);
		}

		m_ComponentsToRemove.clear();
	}

  private:
	std::vector<ComponentToRemove> m_ComponentsToRemove = {};
};