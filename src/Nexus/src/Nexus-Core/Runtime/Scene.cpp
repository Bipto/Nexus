#include "Nexus-Core/Runtime/Scene.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "yaml-cpp/yaml.h"

#include "Nexus-Core/Graphics/HdriProcessor.hpp"
#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/ECS/Components.hpp"

#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Scripting/NativeScript.hpp"

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec2 &rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec3 &rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec4 &rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}	 // namespace YAML

YAML::Emitter &operator<<(YAML::Emitter &output, const glm::vec2 &vec)
{
	output << YAML::Flow;
	output << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
	return output;
}

YAML::Emitter &operator<<(YAML::Emitter &output, const glm::vec3 &vec)
{
	output << YAML::Flow;
	output << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
	return output;
}

YAML::Emitter &operator<<(YAML::Emitter &output, const glm::vec4 &vec)
{
	output << YAML::Flow;
	output << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
	return output;
}

namespace Nexus
{
	Scene::Scene()
	{
		auto graphicsDevice = Nexus::GetApplication()->GetGraphicsDevice();

		Graphics::SamplerDescription samplerSpec = {};
		samplerSpec.AddressModeU				 = Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV				 = Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW				 = Graphics::SamplerAddressMode::Clamp;

		SceneEnvironment.CubemapSampler = graphicsDevice->CreateSampler(samplerSpec);
	}

	void Scene::Serialize(const std::string &filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Scene" << YAML::Value << Name;

			out << YAML::Key << "Environment" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ClearColour" << YAML::Value << SceneEnvironment.ClearColour;
			out << YAML::Key << "Cubemap" << YAML::Value << SceneEnvironment.CubemapPath;
			out << YAML::EndMap;

			out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
			for (const auto &e : GetEntities()) { e.Serialize(out, Registry, ParentProject); }
			out << YAML::EndSeq;
		}

		out << YAML::EndMap;

		FileSystem::WriteFileAbsolute(filepath, out.c_str());
	}

	void Scene::AddEmptyEntity()
	{
		Registry.Create();
	}

	Entity *Scene::GetEntity(GUID id)
	{
		return Registry.GetEntityOrNull(id);
	}

	std::vector<Entity> &Scene::GetEntities()
	{
		return Registry.GetEntities();
	}

	void Scene::Start()
	{
		// we only instantiate scripts if it is the first time clicking the play button
		if (m_SceneState == SceneState::Stopped)
		{
			// instantiate native scripts
			{
				auto view = Registry.GetView<Nexus::NativeScriptComponent>();

				if (view.HasComponents())
				{
					for (auto &[entity, components] : view)
					{
						for (const auto &component : components)
						{
							auto *script = std::get<0>(component);
							script->Instantiate(ParentProject, entity->ID);
						}
					}
				}
			}
		}

		m_SceneState = SceneState::Playing;
	}

	void Scene::Stop()
	{
		m_SceneState = SceneState::Stopped;
	}

	void Scene::Pause()
	{
		m_SceneState = SceneState::Paused;
	}

	void Scene::OnUpdate(TimeSpan time)
	{
		// call native script functions
		{
			auto view = Registry.GetView<Nexus::NativeScriptComponent>();
			if (view.HasComponents())
			{
				for (auto &[entity, components] : view)
				{
					for (const auto &component : components)
					{
						auto *script = std::get<0>(component);
						if (script->ScriptInstance)
						{
							script->ScriptInstance->OnUpdate(time);
						}
					}
				}
			}
		}
	}

	void Scene::OnRender(TimeSpan time)
	{
		// call Native OnRender function
		{
			auto view = Registry.GetView<Nexus::NativeScriptComponent>();

			if (view.HasComponents())
			{
				for (auto &[entity, components] : view)
				{
					for (const auto &component : components)
					{
						auto *script = std::get<0>(component);
						if (script->ScriptInstance)
						{
							script->ScriptInstance->OnRender(time);
						}
					}
				}
			}
		}
	}

	void Scene::OnTick(TimeSpan time)
	{
		// call native OnTick functions
		{
			auto view = Registry.GetView<Nexus::NativeScriptComponent>();

			if (view.HasComponents())
			{
				for (auto &[entity, components] : view)
				{
					for (const auto &component : components)
					{
						auto *script = std::get<0>(component);
						if (script->ScriptInstance)
						{
							script->ScriptInstance->OnTick(time);
						}
					}
				}
			}
		}
	}

	SceneState Scene::GetSceneState()
	{
		return m_SceneState;
	}

	Scene *Scene::Deserialize(const SceneInfo			  &info,
							  const std::string			  &sceneDirectory,
							  Project					  *project,
							  Graphics::GraphicsDevice	  *device,
							  Ref<Graphics::ICommandQueue> commandQueue)
	{
		std::string filepath = sceneDirectory + info.Name + std::string(".scene");

		std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
		YAML::Node	node  = YAML::Load(input);

		if (!node["Scene"])
		{
			return nullptr;
		}

		Scene *scene		 = new Scene();
		scene->Guid			 = info.Guid;
		scene->Name			 = node["Scene"].as<std::string>();
		scene->ParentProject = project;

		auto environment					= node["Environment"];
		scene->SceneEnvironment.ClearColour = environment["ClearColour"].as<glm::vec4>();
		scene->SceneEnvironment.CubemapPath = environment["Cubemap"].as<std::string>();

		if (!scene->SceneEnvironment.CubemapPath.empty() && std::filesystem::exists(scene->SceneEnvironment.CubemapPath))
		{
			Graphics::HdriProcessor processor(scene->SceneEnvironment.CubemapPath, device, commandQueue);
			scene->SceneEnvironment.EnvironmentCubemap = processor.Generate(2048);
		}

		auto entities = node["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t	id	 = entity["Entity"].as<uint64_t>();
				std::string name = entity["Name"].as<std::string>();
				Entity		e(GUID(id), name);
				scene->Registry.AddEntity(e);

				auto components = entity["Components"];
				if (components)
				{
					for (auto component : components)
					{
						Nexus::ECS::ComponentRegistry &componentRegistry = Nexus::ECS::ComponentRegistry::GetRegistry();

						std::string componentName		 = component["Name"].as<std::string>();
						size_t		entityComponentIndex = component["HierarchyIndex"].as<size_t>();
						YAML::Node	data				 = component["Data"];

						project->DeserializeComponentFromYaml(scene->Registry, e.ID, componentName, data, entityComponentIndex);
					}
				}
			}
		}

		return scene;
	}
}	 // namespace Nexus