#include "Scene.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "yaml-cpp/yaml.h"

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
	Scene::Scene(const std::string &name) : m_Name(name)
	{
	}

	void Scene::Serialize(const std::string &filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Name;
		out << YAML::Key << "ClearColour" << YAML::Value << m_ClearColour;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (const auto &e : m_Entities) { e.Serialize(out); }

		out << YAML::EndSeq;
		out << YAML::EndMap;

		FileSystem::WriteFileAbsolute(filepath, out.c_str());
	}

	void Scene::AddEmptyEntity()
	{
		m_Entities.push_back(Entity());
	}

	std::vector<Entity> &Scene::GetEntities()
	{
		return m_Entities;
	}

	Scene *Scene::Deserialize(const std::string &filepath)
	{
		std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
		YAML::Node	node  = YAML::Load(input);

		if (!node["Scene"])
		{
			return nullptr;
		}

		Scene *scene		 = new Scene();
		scene->m_Name		 = node["Scene"].as<std::string>();
		scene->m_ClearColour = node["ClearColour"].as<glm::vec4>();

		auto entities = node["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t	id	 = entity["Entity"].as<uint64_t>();
				std::string name = entity["Name"].as<std::string>();
				Entity		e(GUID(id), name);
				scene->m_Entities.push_back(e);
			}
		}

		return scene;
	}
}	 // namespace Nexus