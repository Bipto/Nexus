#include "Nexus-Core/ECS/Components.hpp"

#include "Nexus-Core/FileSystem/FileDialogs.hpp"

namespace Nexus::Components
{
	void RegisterDefaultComponents()
	{
		ECS::RegisterComponentWithRenderFunc<Nexus::Transform>("Transform",
															   [](void *data, Nexus::Ref<Nexus::Project> project)
															   {
																   Nexus::Transform *transform = static_cast<Nexus::Transform *>(data);
																   ImGui::DragFloat3("Position", glm::value_ptr(transform->Position));
																   ImGui::DragFloat3("Rotation", glm::value_ptr(transform->Rotation));
																   ImGui::DragFloat3("Scale", glm::value_ptr(transform->Scale));
															   });

		ECS::RegisterComponentWithRenderFunc<Nexus::ModelRenderer>(
			"ModelRenderer",
			[](void *data, Nexus::Ref<Nexus::Project> project)
			{
				Nexus::ModelRenderer *modelRenderer = static_cast<Nexus::ModelRenderer *>(data);
				ImGui::Text(modelRenderer->FilePath.c_str());
				ImGui::SameLine();
				ImGui::PushID(Nexus::GUID {});

				ImGui::Button("+");
				if (ImGui::IsItemClicked())
				{
					std::vector<const char *> filters = {
						{"*"},
					};
					const char *filepath = Nexus::FileDialogs::OpenFile(filters);
					if (filepath)
					{
						modelRenderer->FilePath = filepath;
						Graphics::MeshFactory factory(Nexus::GetApplication()->GetGraphicsDevice());
						modelRenderer->Model = factory.CreateFrom3DModelFile(filepath);
					}
				}
				ImGui::PopID();
			});

		ECS::RegisterComponentWithRenderFunc<Nexus::ScriptComponent>("ScriptComponent",
																	 [](void *data, Nexus::Ref<Nexus::Project> project)
																	 {
																		 Nexus::ScriptComponent *component =
																			 static_cast<Nexus::ScriptComponent *>(data);

																		 auto availableScripts = project->GetAvailableScripts();

																		 std::string previewText = "None";
																		 if (!component->ScriptName.empty())
																		 {
																			 previewText = component->ScriptName;
																		 }

																		 if (ImGui::BeginCombo("Script", previewText.c_str()))
																		 {
																			 if (ImGui::Selectable("None"))
																			 {
																				 component->ScriptName = "";
																			 }

																			 for (const auto &[name, creationFunc] : availableScripts)
																			 {
																				 if (ImGui::Selectable(name.c_str()))
																				 {
																					 component->ScriptName = name;
																				 }
																			 }

																			 ImGui::EndCombo();
																		 }
																	 });
	}
}	 // namespace Nexus::Components