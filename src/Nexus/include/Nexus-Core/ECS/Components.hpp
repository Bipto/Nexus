#pragma once

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Scripting/Script.hpp"

#include "Nexus-Core/FileSystem/FileDialogs.hpp"

#include "ComponentRegistry.hpp"

namespace Nexus
{
	struct Transform
	{
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Scale	   = glm::vec3(1.0f, 1.0f, 1.0f);

		inline glm::mat4 CreateTransformation()
		{
			glm::mat4 transformation(1.0f);

			// apply translation
			transformation = glm::translate(transformation, Position);

			// apply rotation
			transformation = glm::rotate(transformation, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			transformation = glm::rotate(transformation, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transformation = glm::rotate(transformation, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			// apply scale
			transformation = glm::scale(transformation, Scale);

			return transformation;
		}

		friend std::ostream &operator<<(std::ostream &os, const Transform &transform)
		{
			os << transform.Position.x << " " << transform.Position.y << " " << transform.Position.z << " ";
			os << transform.Rotation.x << " " << transform.Rotation.y << " " << transform.Rotation.z << " ";
			os << transform.Scale.x << " " << transform.Scale.y << " " << transform.Scale.z;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, Transform &transform)
		{
			is >> transform.Position.x >> transform.Position.y >> transform.Position.z;
			is >> transform.Rotation.x >> transform.Rotation.y >> transform.Rotation.z;
			is >> transform.Scale.x >> transform.Scale.y >> transform.Scale.z;

			return is;
		}
	};

	NX_REGISTER_COMPONENT(Transform,
						  [](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  Nexus::Transform *transform = static_cast<Nexus::Transform *>(data);
							  ImGui::DragFloat3("Position", glm::value_ptr(transform->Position));
							  ImGui::DragFloat3("Rotation", glm::value_ptr(transform->Rotation));
							  ImGui::DragFloat3("Scale", glm::value_ptr(transform->Scale));
						  });

	struct ModelRenderer
	{
		std::string						   FilePath = {};
		Nexus::Ref<Nexus::Graphics::Model> Model	= {};

		friend std::ostream &operator<<(std::ostream &os, const ModelRenderer &modelRenderer)
		{
			os << modelRenderer.FilePath;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, ModelRenderer &modelRenderer)
		{
			is >> modelRenderer.FilePath;

			if (!modelRenderer.FilePath.empty())
			{
				if (std::filesystem::exists(modelRenderer.FilePath))
				{
					Graphics::MeshFactory factory(Nexus::GetApplication()->GetGraphicsDevice());
					modelRenderer.Model = factory.CreateFrom3DModelFile(modelRenderer.FilePath);
				}
			}

			return is;
		}
	};

	NX_REGISTER_COMPONENT(ModelRenderer,

						  [](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  Nexus::ModelRenderer *renderer = static_cast<Nexus::ModelRenderer *>(data);
							  ImGui::Text(renderer->FilePath.c_str());
							  ImGui::SameLine();
							  ImGui::PushID(Nexus::GUID {});

							  ImGui::Button("+");
							  if (ImGui::IsItemClicked())
							  {
								  std::vector<const char *> filters = {"*"};

								  const char *filepath = Nexus::FileDialogs::OpenFile(filters);
								  if (filepath)
								  {
									  renderer->FilePath = filepath;
									  Graphics::MeshFactory factory(Nexus::GetApplication()->GetGraphicsDevice());
									  renderer->Model = factory.CreateFrom3DModelFile(filepath);
								  }
							  }

							  ImGui::PopID();
						  });

	struct ScriptComponent
	{
		std::string				  ScriptName	 = "";
		Nexus::Scripting::Script *ScriptInstance = nullptr;

		virtual ~ScriptComponent()
		{
			if (!ScriptInstance)
			{
				delete ScriptInstance;
			}
		}

		inline void Instantiate(Nexus::Project *project, Nexus::GUID guid)
		{
			if (!project)
			{
				return;
			}

			auto scripts = project->LoadAvailableScripts();
			for (const auto &[name, creationFunction] : scripts)
			{
				if (name == ScriptName)
				{
					ScriptInstance = creationFunction();
					ScriptInstance->Instantiate(project, guid);
				}
			}
		}

		friend std::ostream &operator<<(std::ostream &os, const ScriptComponent &component)
		{
			os << component.ScriptName;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, ScriptComponent &component)
		{
			is >> component.ScriptName;
			return is;
		}
	};

	NX_REGISTER_COMPONENT(ScriptComponent,

						  [](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  if (!project)
								  return;

							  Nexus::ScriptComponent *component = static_cast<Nexus::ScriptComponent *>(data);

							  auto availableScripts = project->GetCachedAvailableScripts();

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

								  for (const auto &name : availableScripts)
								  {
									  if (ImGui::Selectable(name.c_str()))
									  {
										  component->ScriptName = name;
									  }
								  }

								  ImGui::EndCombo();
							  }
						  });

}	 // namespace Nexus