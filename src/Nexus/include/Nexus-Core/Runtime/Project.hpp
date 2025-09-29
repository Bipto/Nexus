#pragma once

#include "Nexus-Core/AssetManager.hpp"
#include "Nexus-Core/Assets/AssetRegistry.hpp"
#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/Utils/SharedLibrary.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Scene.hpp"

namespace Nexus
{
	namespace Scripting
	{
		class NativeScript;
	}

	namespace Graphics
	{
		class GraphicsDevice;
		class ICommandQueue;
	}	 // namespace Graphics

	class NX_API Project
	{
	  public:
		Project(Graphics::GraphicsDevice	*device,
				Ref<Graphics::ICommandQueue> commandQueue,
				const std::string			&name,
				const std::string			&directory,
				bool						 createDefaultScene);
		Project() = default;
		~Project();
		void				Serialize();
		static Ref<Project> Deserialize(const std::string &filepath, Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);

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

		const std::vector<SceneInfo> &GetScenes() const;
		size_t						  GetNumberOfScenes() const;
		bool						  IsSceneLoaded() const;

		void LoadScene(uint32_t index, Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		void LoadScene(const std::string &name, Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		void CreateNewScene(const std::string &name);
		void ReloadCurrentScene(Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);

		void OnUpdate(TimeSpan time);
		void OnRender(TimeSpan time);
		void OnTick(TimeSpan time);

		std::string GetFullSceneDirectory();
		std::string GetFullAssetsDirectory();
		std::string GetFullScriptsDirectory();

		// project library
		void				  LoadSharedLibrary();
		Utils::SharedLibrary *GetSharedLibrary();
		void				  UnloadSharedLibrary();
		void				  LoadDataFromSharedLibrary();

		// script functions
		std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> LoadAvailableScripts();
		void																	 CacheAvailableScripts();
		const std::vector<std::string>											&GetCachedAvailableScripts() const;

		// component functions
		std::map<std::string, ECS::ComponentStorage>		LoadAvailableComponents();
		void												CacheAvailableComponents();
		const std::map<std::string, ECS::ComponentStorage> &GetCachedAvailableComponents() const;

		void													LoadAvailableAssetProcessors();
		const std::map<std::string, Processors::ProcessorInfo> &GetCachedAvailableAssetProcessors() const;
		std::optional<Processors::ProcessorInfo>				GetProcessorInfo(const std::string &name);

		std::string GetComponentDisplayNameFromTypeName(const std::string &typeName) const;
		std::string GetComponentTypeNameFromDisplayName(const std::string &displayName) const;
		std::string GetDisplayNameFromComponent(ECS::ComponentPtr component) const;

		void		RenderComponentUI(ECS::Registry &registry, ECS::ComponentPtr component, Nexus::Ref<Nexus::Project> project);
		std::string SerializeComponentToString(ECS::Registry &registry, ECS::ComponentPtr component);
		void		DeserializeComponentFromString(ECS::Registry	 &registry,
												   GUID				  guid,
												   const std::string &displayName,
												   const std::string &data,
												   size_t			  entityHierarchyIndex);
		YAML::Node	SerializeComponentToYaml(ECS::Registry &registry, ECS::ComponentPtr component);
		void		DeserializeComponentFromYaml(ECS::Registry	   &registry,
												 GUID				guid,
												 const std::string &displayName,
												 const YAML::Node  &node,
												 size_t				entityHierarchyIndex);

		void				   CreateComponent(const char *typeName, ECS::Registry &registry, const Entity &entity);
		Assets::AssetRegistry &GetAssetRegistry();

	  private:
		void WriteProjectFile();
		void WriteSceneFile(const std::string &sceneDirectory);

	  private:
		std::string m_Name			   = {};
		std::string m_RootDirectory	   = {};
		std::string m_SceneDirectory   = {};
		std::string m_AssetsDirectory  = {};
		std::string m_ScriptsDirectory = {};

		std::vector<SceneInfo> m_Scenes = {};

		std::unique_ptr<Scene> m_LoadedScene  = nullptr;
		uint32_t			   m_StartupScene = 0;

		Nexus::Utils::SharedLibrary						*m_Library					= nullptr;
		std::vector<std::string>						 m_AvailableScripts			= {};
		std::map<std::string, ECS::ComponentStorage>	 m_AvailableComponents		= {};
		std::map<std::string, Processors::ProcessorInfo> m_AvailableAssetProcessors = {};
		Assets::AssetRegistry							 m_AssetRegistry			= {};

		std::unique_ptr<AssetManager> m_AssetManager = nullptr;
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