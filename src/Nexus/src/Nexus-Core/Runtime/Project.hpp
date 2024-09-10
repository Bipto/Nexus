#pragma once

#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Scene.hpp"

namespace Nexus
{
	struct SceneInfo
	{
		std::string Name;
		std::string Path;
	};

	class Project
	{
	  public:
		Project(const std::string &name = "Untitled Project", const std::string &directory = "");
		void				Serialize();
		static Ref<Project> Deserialize(const std::string &filepath);

		Scene *GetLoadedScene()
		{
			return m_LoadedScene.get();
		}

		const std::string &GetName() const
		{
			return m_Name;
		}
		void SetName(const std::string &name)
		{
			m_Name = name;
		}

		void LoadScene(uint32_t index);
		void LoadScene(const std::string &name);
		void CreateNewScene(const std::string &name);

	  public:
		static Ref<Project> s_ActiveProject;

	  private:
		void WriteProjectFile();
		void WriteSceneFile(const std::string &sceneDirectory);

	  private:
		std::string m_Name			  = {};
		std::string m_RootDirectory	  = {};
		std::string m_SceneDirectory  = {};
		std::string m_AssetsDirectory = {};

		std::vector<SceneInfo> m_Scenes;

		std::unique_ptr<Scene> m_LoadedScene;
		uint32_t			   m_StartupScene = 0;
	};
}	 // namespace Nexus

namespace YAML
{
	template<>
	struct convert<Nexus::SceneInfo>
	{
		static Node encode(const Nexus::SceneInfo &rhs)
		{
			Node node;
			node.push_back(rhs.Name);
			node.push_back(rhs.Path);
			return node;
		}

		static bool decode(const Node &node, Nexus::SceneInfo &rhs)
		{
			if (node.IsSequence() || node.size() != 2)
			{
				rhs.Name = node[0].as<std::string>();
				rhs.Path = node[1].as<std::string>();
				return true;
			}

			return false;
		}
	};
}	 // namespace YAML