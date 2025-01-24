#pragma once

#include "ComponentRegistry.hpp"
#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Registry.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/Scripting/Script.hpp"

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

			auto scripts = project->GetAvailableScripts();
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

}	 // namespace Nexus

namespace Nexus::Components
{
	void RegisterDefaultComponents();
}