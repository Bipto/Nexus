#pragma once

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Scripting/NativeScript.hpp"

#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/Platform.hpp"

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

		friend YAML::Node &operator>>(YAML::Node &node, Transform &person)
		{
			return node;
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

		inline void LoadModel()
		{
			if (!FilePath.empty())
			{
				if (std::filesystem::exists(FilePath))
				{
					Graphics::MeshFactory factory(Nexus::GetApplication()->GetGraphicsDevice(), nullptr);
					Model = factory.CreateFrom3DModelFile(FilePath);
				}
			}
		}

		friend std::ostream &operator<<(std::ostream &os, const ModelRenderer &modelRenderer)
		{
			os << modelRenderer.FilePath;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, ModelRenderer &modelRenderer)
		{
			is >> modelRenderer.FilePath;
			modelRenderer.LoadModel();
			return is;
		}
	};

	NX_REGISTER_COMPONENT(ModelRenderer,

						  [](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  Nexus::ModelRenderer *renderer = static_cast<Nexus::ModelRenderer *>(data);
							  ImGui::Text("%s", renderer->FilePath.c_str());
							  ImGui::SameLine();
							  ImGui::PushID(Nexus::GUID {});

							  ImGui::Button("+");
							  if (ImGui::IsItemClicked())
							  {
								  std::vector<Nexus::FileDialogFilter> filters;
								  filters.push_back(Nexus::FileDialogFilter {.Name = "All files", .Pattern = "*"});

								  std::unique_ptr<Nexus::OpenFileDialog> dialog = std::unique_ptr<Nexus::OpenFileDialog>(
									  Nexus::Platform::CreateOpenFileDialog(Nexus::GetApplication()->GetPrimaryWindow(), filters, nullptr, false));
								  Nexus::FileDialogResult result = dialog->Show();

								  if (result.FilePaths.size() > 0)
								  {
									  std::string filepath = result.FilePaths[0];
									  renderer->FilePath   = filepath;
									  renderer->LoadModel();
								  }
							  }

							  ImGui::PopID();
						  });

	struct NativeScriptComponent
	{
		std::string						ScriptName	   = "";
		Nexus::Scripting::NativeScript *ScriptInstance = nullptr;

		virtual ~NativeScriptComponent()
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

		friend std::ostream &operator<<(std::ostream &os, const NativeScriptComponent &component)
		{
			os << component.ScriptName;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, NativeScriptComponent &component)
		{
			is >> component.ScriptName;
			return is;
		}
	};

	NX_REGISTER_COMPONENT(NativeScriptComponent,

						  [](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  if (!project)
								  return;

							  Nexus::NativeScriptComponent *component = static_cast<Nexus::NativeScriptComponent *>(data);

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

	struct SpriteRendererComponent
	{
		std::string			   TexturePath	 = {};
		GUID				   TextureID	 = GUID(0);
		Ref<Graphics::Texture> SpriteTexture = nullptr;
		glm::vec4			   SpriteColour	 = {1.0f, 1.0f, 1.0f, 1.0f};
		float				   Tiling		 = 1.0f;

		friend std::ostream &operator<<(std::ostream &os, const SpriteRendererComponent &component)
		{
			os << component.TexturePath;
			os << component.SpriteColour.r << " " << component.SpriteColour.g << " " << component.SpriteColour.b << " " << component.SpriteColour.a;
			os << component.Tiling;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, SpriteRendererComponent &component)
		{
			is >> component.TexturePath;
			is >> component.SpriteColour.r >> component.SpriteColour.g >> component.SpriteColour.b >> component.SpriteColour.a;
			is >> component.Tiling;
			return is;
		}

		inline void LoadTexture()
		{
			if (!TexturePath.empty())
			{
				Nexus::Graphics::GraphicsDevice *device = Nexus::GetApplication()->GetGraphicsDevice();
				SpriteTexture							= device->CreateTexture2D(nullptr, TexturePath, true, false);
			}
		}
	};

	NX_REGISTER_COMPONENT(SpriteRendererComponent,

						  [&](void *data, Nexus::Ref<Nexus::Project> project)
						  {
							  if (!project)
								  return;

							  Nexus::SpriteRendererComponent *component = static_cast<Nexus::SpriteRendererComponent *>(data);

							  ImGui::ColorEdit4("Colour", glm::value_ptr(component->SpriteColour));
							  ImGui::DragFloat("Tiling", &component->Tiling);

							  ImGui::Text("%s", component->TexturePath.c_str());
							  ImGui::SameLine();
							  ImGui::PushID(Nexus::GUID {});

							  ImGui::Button("+");
							  if (ImGui::IsItemClicked())
							  {
								  std::vector<Nexus::FileDialogFilter> filters;
								  filters.push_back(Nexus::FileDialogFilter {.Name = "All files", .Pattern = "*"});

								  std::unique_ptr<Nexus::OpenFileDialog> dialog = std::unique_ptr<Nexus::OpenFileDialog>(
									  Nexus::Platform::CreateOpenFileDialog(Nexus::GetApplication()->GetPrimaryWindow(), filters, nullptr, false));
								  Nexus::FileDialogResult result = dialog->Show();

								  if (result.FilePaths.size() > 0)
								  {
									  std::string filepath	 = result.FilePaths[0];
									  component->TexturePath = filepath;
									  component->LoadTexture();
								  }
							  }

							  ImGui::PopID();
						  });

}	 // namespace Nexus

namespace YAML
{
	template<>
	struct convert<Nexus::Transform>
	{
		static Node encode(const Nexus::Transform &rhs)
		{
			Node node;
			node["Position"]["X"] = rhs.Position.x;
			node["Position"]["Y"] = rhs.Position.y;
			node["Position"]["Z"] = rhs.Position.z;
			node["Rotation"]["X"] = rhs.Rotation.x;
			node["Rotation"]["Y"] = rhs.Rotation.y;
			node["Rotation"]["Z"] = rhs.Rotation.z;
			node["Scale"]["X"]	  = rhs.Scale.x;
			node["Scale"]["Y"]	  = rhs.Scale.y;
			node["Scale"]["Z"]	  = rhs.Scale.z;
			return node;
		}

		static bool decode(const Node &node, Nexus::Transform &rhs)
		{
			if (!node["Position"] || !node["Rotation"] || !node["Scale"])
			{
				return false;
			}

			rhs.Position.x = node["Position"]["X"].as<float>();
			rhs.Position.y = node["Position"]["Y"].as<float>();
			rhs.Position.z = node["Position"]["Z"].as<float>();
			rhs.Rotation.x = node["Rotation"]["X"].as<float>();
			rhs.Rotation.y = node["Rotation"]["Y"].as<float>();
			rhs.Rotation.z = node["Rotation"]["Z"].as<float>();
			rhs.Scale.x	   = node["Scale"]["X"].as<float>();
			rhs.Scale.y	   = node["Scale"]["Y"].as<float>();
			rhs.Scale.z	   = node["Scale"]["Z"].as<float>();

			return true;
		}
	};

	template<>
	struct convert<Nexus::ModelRenderer>
	{
		static Node encode(const Nexus::ModelRenderer &rhs)
		{
			Node node;
			node["Filepath"] = rhs.FilePath;
			return node;
		}

		static bool decode(const Node &node, Nexus::ModelRenderer &rhs)
		{
			if (!node["Filepath"])
			{
				return false;
			}

			rhs.FilePath = node["Filepath"].as<std::string>();
			rhs.LoadModel();
			return true;
		}
	};

	template<>
	struct convert<Nexus::NativeScriptComponent>
	{
		static Node encode(const Nexus::NativeScriptComponent &rhs)
		{
			Node node;
			node["ScriptName"] = rhs.ScriptName;
			return node;
		}

		static bool decode(const Node &node, Nexus::NativeScriptComponent &rhs)
		{
			if (!node["ScriptName"])
			{
				return false;
			}

			rhs.ScriptName = node["ScriptName"].as<std::string>();
			return true;
		}
	};

	template<>
	struct convert<Nexus::SpriteRendererComponent>
	{
		static Node encode(const Nexus::SpriteRendererComponent &rhs)
		{
			Node node;
			node["Filepath"]	= rhs.TexturePath;
			node["Colour"]["r"] = rhs.SpriteColour.r;
			node["Colour"]["g"] = rhs.SpriteColour.g;
			node["Colour"]["b"] = rhs.SpriteColour.b;
			node["Colour"]["a"] = rhs.SpriteColour.a;
			node["Tiling"]		= rhs.Tiling;
			return node;
		}

		static bool decode(const Node &node, Nexus::SpriteRendererComponent &rhs)
		{
			if (!node["Filepath"] || !node["Colour"] || !node["Tiling"])
			{
				return false;
			}
			rhs.TexturePath = node["Filepath"].as<std::string>();

			rhs.SpriteColour.r = node["Colour"]["r"].as<float>();
			rhs.SpriteColour.g = node["Colour"]["g"].as<float>();
			rhs.SpriteColour.b = node["Colour"]["b"].as<float>();
			rhs.SpriteColour.a = node["Colour"]["a"].as<float>();

			rhs.Tiling = node["Tiling"].as<float>();

			rhs.LoadTexture();
			return true;
		}
	};
}	 // namespace YAML