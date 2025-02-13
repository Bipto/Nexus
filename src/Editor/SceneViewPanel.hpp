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
				std::vector<Nexus::FileDialogFilter>   filters = {{"HDR images", "hdr"}};
				std::unique_ptr<Nexus::OpenFileDialog> dialog  = std::unique_ptr<Nexus::OpenFileDialog>(
					 Nexus::Platform::CreateOpenFileDialog(Nexus::GetApplication()->GetPrimaryWindow(), filters, nullptr, false));
				Nexus::FileDialogResult result = dialog->Show();
				if (result.FilePaths.size() > 0)
				{
					std::string file					= result.FilePaths[0];
					scene->SceneEnvironment.CubemapPath = file;

					auto		graphicsDevice = Nexus::GetApplication()->GetGraphicsDevice();
					std::string path		   = file;
					if (!path.empty() && std::filesystem::exists(path))
					{
						Nexus::Graphics::HdriProcessor processor(path, graphicsDevice);
						scene->SceneEnvironment.EnvironmentCubemap = processor.Generate(2048);
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("Entities");
			for (auto &entity : scene->GetEntities())
			{
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