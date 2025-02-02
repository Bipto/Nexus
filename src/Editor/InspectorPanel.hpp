#pragma once

#include "Panel.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"

struct ComponentToAdd
{
	Nexus::ECS::CreateComponentFunc createFunc = nullptr;
	Nexus::Entity				   *entity	   = nullptr;
};

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
			Nexus::ECS::ComponentRegistry &registry = Nexus::ECS::ComponentRegistry::GetRegistry();

			if (ImGui::BeginPopupContextItem("Add Component"))
			{
				const std::map<std::string, Nexus::ECS::ComponentStorage> &registeredComponents = m_Project->GetCachedAvailableComponents();

				ImGui::Text("Add Components");
				ImGui::Separator();

				for (const auto &[name, storage] : registeredComponents)
				{
					std::string displayName = registry.GetDisplayNameFromTypeName(name);
					if (ImGui::Selectable(displayName.c_str()))
					{
						ComponentToAdd newComponent = {.createFunc = storage.CreationFunction, .entity = entity};
						m_ComponentsToAdd.push_back(newComponent);
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
			std::sort(components.begin(),
					  components.end(),
					  [](const Nexus::ECS::ComponentPtr &a, const Nexus::ECS::ComponentPtr &b)
					  { return a.entityComponentIndex < b.entityComponentIndex; });

			for (Nexus::ECS::ComponentPtr component : components)
			{
				void *obj = scene->Registry.GetRawComponent(component);
				ImGui::PushID(obj);
				std::string displayName = registry.GetDisplayNameFromTypeName(component.typeName);
				ImGui::Text(displayName.c_str());

				registry.RenderComponent(scene->Registry, component, m_Project);

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
		if (!m_Project || !m_Project->IsSceneLoaded())
		{
			return;
		}

		Nexus::Scene *scene = m_Project->GetLoadedScene();

		for (const auto &component : m_ComponentsToRemove)
		{
			scene->Registry.RemoveComponent(component.EntityID, component.TypeName, component.ComponentIndex);
		}

		for (auto &component : m_ComponentsToAdd) { component.createFunc(scene->Registry, *component.entity); }

		m_ComponentsToAdd.clear();
		m_ComponentsToRemove.clear();
	}

  private:
	std::map<std::string, std::function<Nexus::Scripting::Script *()>> m_AvailableScripts	= {};
	std::vector<ComponentToAdd>	   m_ComponentsToAdd	= {};
	std::vector<ComponentToRemove> m_ComponentsToRemove = {};
};