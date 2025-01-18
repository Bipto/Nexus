#pragma once

#include "Panel.hpp"

class SceneViewPanel : public Panel
{
  public:
	Nexus::EventHandler<std::optional<Nexus::GUID>> OnEntitySelected;

  public:
	SceneViewPanel() : Panel("Scene")
	{
	}

	void Render() final
	{
		ImGui::Begin(m_Name.c_str(), &m_Open);

		if (m_Project && m_Project->IsSceneLoaded())
		{
			Nexus::Scene *scene = m_Project->GetLoadedScene();

			ImGui::Text("Scene");
			ImGui::InputText("Name", &scene->Name);

			ImGui::Separator();
			ImGui::Text("Environment");
			ImGui::ColorEdit4("ClearColour", glm::value_ptr(scene->SceneEnvironment.ClearColour));

			std::string text = "Cubemap: " + scene->SceneEnvironment.CubemapPath;
			ImGui::Text(text.c_str());
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				/* std::vector<Nexus::FileDialogs::Filter> filters = {
					{"HDRI images", "hdr"},
				};
				const char *file = Nexus::FileDialogs::OpenFile(filters, Nexus::GetApplication()->GetPrimaryWindow());
				if (file)
				{
					scene->SceneEnvironment.CubemapPath = file;

					auto		graphicsDevice = Nexus::GetApplication()->GetGraphicsDevice();
					std::string path		   = file;
					if (!path.empty() && std::filesystem::exists(path))
					{
						Nexus::Graphics::HdriProcessor processor(path, graphicsDevice);
						scene->SceneEnvironment.EnvironmentCubemap = processor.Generate(2048);
					}
				} */
			}

			ImGui::Separator();
			ImGui::Text("Entities");
			for (auto &entity : scene->GetEntities())
			{
				/* std::stringstream ss;
				ss << "ID:" << entity.ID;
				ImGui::Text(ss.str().c_str());
				ImGui::PushID(entity.ID);
				ImGui::InputText("Name", &entity.Name);
				ImGui::PopID(); */

				ImGui::PushID(entity.ID);
				if (ImGui::Selectable(entity.Name.c_str()))
				{
					OnEntitySelected.Invoke(entity.ID);
				}
				ImGui::PopID();
			}

			if (ImGui::Button("New Entity"))
			{
				scene->AddEmptyEntity();
			}
		}

		ImGui::End();
	}
};