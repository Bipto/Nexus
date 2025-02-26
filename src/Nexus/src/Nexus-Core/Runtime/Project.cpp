#include "Nexus-Core/Runtime/Project.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "yaml-cpp/yaml.h"

#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/Scripting/NativeScript.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

const std::string DefaultSceneName = "UntitledScene";

namespace Nexus
{
	NX_API Ref<Project> Project::s_ActiveProject = nullptr;

	Project::Project(const std::string &name, const std::string &directory, bool createDefaultScene)
		: m_Name(name),
		  m_RootDirectory(directory + std::string("\\") + name)
	{
		m_SceneDirectory   = "\\Scenes\\";
		m_AssetsDirectory  = "\\Assets\\";
		m_ScriptsDirectory = "\\Scripts\\";

		if (createDefaultScene)
		{
			CreateNewScene(DefaultSceneName);
		}

		LoadSharedLibrary();
		LoadDataFromSharedLibrary();
	}

	Project::~Project()
	{
		UnloadSharedLibrary();
	}

	void Project::Serialize()
	{
		WriteProjectFile();
		WriteSceneFile(GetFullSceneDirectory());

		std::string assetsDirectory = m_RootDirectory + m_AssetsDirectory;
		Nexus::FileSystem::CreateFileDirectory(assetsDirectory);
	}

	Ref<Project> Project::Deserialize(const std::string &filepath)
	{
		std::string directory = std::filesystem::path(filepath).parent_path().string();

		std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
		YAML::Node	node  = YAML::Load(input);

		Ref<Project> project	 = CreateRef<Project>();
		project->m_Name			 = node["Project"].as<std::string>();
		project->m_RootDirectory = directory;

		auto scenes = node["Scenes"];
		if (scenes)
		{
			for (auto scene : scenes)
			{
				SceneInfo sceneInfo;
				sceneInfo.Guid = GUID(scene["GUID"].as<uint64_t>());
				sceneInfo.Name = scene["Name"].as<std::string>();
				sceneInfo.Path = scene["Path"].as<std::string>();
				project->m_Scenes.push_back(sceneInfo);
			}
		}

		project->m_StartupScene		= node["StartupScene"].as<uint32_t>();
		project->m_SceneDirectory	= node["SceneDirectory"].as<std::string>();
		project->m_AssetsDirectory	= node["AssetsDirectory"].as<std::string>();
		project->m_ScriptsDirectory = node["ScriptsDirectory"].as<std::string>();

		project->LoadSharedLibrary();
		project->LoadDataFromSharedLibrary();
		project->LoadScene(project->m_StartupScene);

		return project;
	}

	const std::vector<SceneInfo> &Project::GetScenes() const
	{
		return m_Scenes;
	}

	size_t Project::GetNumberOfScenes() const
	{
		return m_Scenes.size();
	}

	bool Project::IsSceneLoaded() const
	{
		return m_LoadedScene != nullptr;
	}

	void Project::LoadScene(uint32_t index)
	{
		if (m_Scenes.size() > index)
		{
			const SceneInfo &info  = m_Scenes.at(index);
			Scene			*scene = Scene::Deserialize(info, GetFullSceneDirectory(), this);
			m_LoadedScene		   = std::unique_ptr<Scene>(scene);
		}
	}

	void Project::LoadScene(const std::string &name)
	{
		for (size_t i = 0; i < m_Scenes.size(); i++)
		{
			const auto &sceneInfo = m_Scenes.at(i);
			if (sceneInfo.Name == name)
			{
				LoadScene(i);
				return;
			}
		}
	}

	void Project::CreateNewScene(const std::string &name)
	{
		std::string path = m_SceneDirectory + "\\" + name + ".scene";

		SceneInfo info;
		info.Guid = {};
		info.Name = name;
		info.Path = path;
		m_Scenes.push_back(info);

		m_LoadedScene		   = std::make_unique<Scene>();
		m_LoadedScene->Guid	   = info.Guid;
		m_LoadedScene->Name	   = info.Name;
		m_LoadedScene->Project = this;
	}

	void Project::ReloadCurrentScene()
	{
		LoadScene(m_LoadedScene->Name);
	}

	void Project::OnUpdate(TimeSpan time)
	{
		if (!m_LoadedScene || m_LoadedScene->GetSceneState() != SceneState::Playing)
		{
			return;
		}

		m_LoadedScene->OnUpdate(time);
	}

	void Project::OnRender(TimeSpan time)
	{
		if (!m_LoadedScene || m_LoadedScene->GetSceneState() != SceneState::Playing)
		{
			return;
		}

		m_LoadedScene->OnRender(time);
	}

	void Project::OnTick(TimeSpan time)
	{
		if (!m_LoadedScene || m_LoadedScene->GetSceneState() != SceneState::Playing)
		{
			return;
		}

		m_LoadedScene->OnTick(time);
	}

	std::string Project::GetFullSceneDirectory()
	{
		return m_RootDirectory + m_SceneDirectory;
	}

	std::string Project::GetFullAssetsDirectory()
	{
		return m_RootDirectory + m_AssetsDirectory;
	}

	void Project::LoadSharedLibrary()
	{
		std::string sharedLibraryPathFile = m_RootDirectory + m_ScriptsDirectory + "loading\\output.txt";

		if (!std::filesystem::exists(sharedLibraryPathFile))
		{
			return;
		}

		std::string libraryPath		= Nexus::FileSystem::ReadFileToString(sharedLibraryPathFile);
		std::string fullLibraryPath = Nexus::StringUtils::RemoveCharacter(libraryPath, '\n');
		if (fullLibraryPath.empty() || !std::filesystem::exists(fullLibraryPath))
		{
			return;
		}

		m_Library = Platform::LoadSharedLibrary(fullLibraryPath);

		if (m_Library)
		{
			typedef void (*SharedEngineStateFunc)(Nexus::Application *, ImGuiContext *, ImGuiMemAllocFunc, ImGuiMemFreeFunc);
			SharedEngineStateFunc func = (SharedEngineStateFunc)m_Library->LoadSymbol("ShareEngineState");
			if (func)
			{
				ImGuiContext							 *context		= nullptr;
				Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer = Nexus::ImGuiUtils::ImGuiGraphicsRenderer::GetCurrentRenderer();
				if (imGuiRenderer)
				{
					context = imGuiRenderer->GetContext();
				}

				Nexus::Application *app = Nexus::GetApplication();
				func(app, context, ImGuiAlloc, ImGuiFree);
			}
		}
	}

	Utils::SharedLibrary *Project::GetSharedLibrary()
	{
		return m_Library;
	}

	std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> Project::LoadAvailableScripts()
	{
		typedef std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> &(*GetScriptRegistryFunc)();

		std::map<std::string, std::function<Nexus::Scripting::NativeScript *()>> scripts = Nexus::Scripting::GetScriptRegistry();

		if (m_Library)
		{
			GetScriptRegistryFunc func = (GetScriptRegistryFunc)m_Library->LoadSymbol("GetScriptRegistry");
			if (func)
			{
				auto loadedScripts = func();
				for (const auto &[name, creationFunc] : loadedScripts) { scripts[name] = creationFunc; }
			}
		}

		return scripts;
	}

	void Project::CacheAvailableScripts()
	{
		auto scripts = LoadAvailableScripts();
		m_AvailableScripts.clear();
		m_AvailableScripts.reserve(scripts.size());

		for (const auto &[name, creationFunc] : scripts) { m_AvailableScripts.push_back(name); }
	}

	const std::vector<std::string> &Project::GetCachedAvailableScripts() const
	{
		return m_AvailableScripts;
	}

	void Project::UnloadSharedLibrary()
	{
		delete m_Library;
		m_Library = nullptr;
	}

	void Project::LoadDataFromSharedLibrary()
	{
		// LoadSharedLibrary();
		CacheAvailableScripts();
		CacheAvailableComponents();
		// UnloadSharedLibrary();
	}

	std::map<std::string, ECS::ComponentStorage> Project::LoadAvailableComponents()
	{
		typedef ECS::ComponentRegistry &(*GetComponentRegistryFunc)();

		std::map<std::string, ECS::ComponentStorage> components;

		// find components with core library
		{
			ECS::ComponentRegistry &componentRegistry = ECS::ComponentRegistry::GetRegistry();
			for (const auto &[name, storage] : componentRegistry.GetRegisteredComponents()) { components[name] = storage; }
		}

		// find components within project library
		{
			if (m_Library)
			{
				GetComponentRegistryFunc func = (GetComponentRegistryFunc)m_Library->LoadSymbol("GetComponentRegistry");
				if (func)
				{
					ECS::ComponentRegistry &componentRegistry = func();
					for (const auto &[name, storage] : componentRegistry.GetRegisteredComponents()) { components[name] = storage; }
				}
			}
		}
		return components;
	}

	void Project::CacheAvailableComponents()
	{
		auto components = LoadAvailableComponents();
		m_AvailableComponents.clear();
		m_AvailableComponents = components;
	}

	const std::map<std::string, ECS::ComponentStorage> &Project::GetCachedAvailableComponents() const
	{
		return m_AvailableComponents;
	}

	std::string Project::GetComponentDisplayNameFromTypeName(const std::string &typeName) const
	{
		for (const auto &[name, storage] : m_AvailableComponents)
		{
			if (name == typeName)
			{
				return storage.DisplayName;
			}
		}

		throw std::runtime_error("Could not find component display name");
	}

	std::string Project::GetComponentTypeNameFromDisplayName(const std::string &displayName) const
	{
		for (auto &[typeName, storage] : m_AvailableComponents)
		{
			if (storage.DisplayName == displayName)
			{
				return typeName;
			}
		}

		throw std::runtime_error("Could not find a component type name");
	}

	std::string Project::GetDisplayNameFromComponent(ECS::ComponentPtr component) const
	{
		if (m_AvailableComponents.find(component.typeName) != m_AvailableComponents.end())
		{
			auto &storage = m_AvailableComponents.at(component.typeName);
			return storage.DisplayName;
		}

		throw std::runtime_error("Component does not have a display name");
	}

	void Project::RenderComponentUI(Nexus::ECS::Registry &registry, Nexus::ECS::ComponentPtr component, Nexus::Ref<Nexus::Project> project)
	{
		if (m_AvailableComponents.find(component.typeName) != m_AvailableComponents.end())
		{
			auto &storage = m_AvailableComponents.at(component.typeName);
			void *obj	  = registry.GetRawComponent(component.typeName, component.componentIndex);
			storage.RenderFunc(obj, project);
			return;
		}

		throw std::runtime_error("Could not find component display name");
	}

	std::string Project::SerializeComponentToString(ECS::Registry &registry, ECS::ComponentPtr component)
	{
		const char *typeName = component.typeName;

		if (m_AvailableComponents.find(typeName) != m_AvailableComponents.end())
		{
			auto &componentStorage = m_AvailableComponents.at(typeName);
			void *obj			   = registry.GetRawComponent(component.typeName, component.componentIndex);
			return componentStorage.StringSerializer(obj);
		}

		throw std::runtime_error("Type is not registed for serialization");
	}

	void Project::DeserializeComponentFromString(ECS::Registry	   &registry,
												 GUID				guid,
												 const std::string &displayName,
												 const std::string &data,
												 size_t				entityHierarchyIndex)
	{
		std::string typeName = GetComponentTypeNameFromDisplayName(displayName);

		if (m_AvailableComponents.find(typeName.c_str()) != m_AvailableComponents.end())
		{
			auto &storage = m_AvailableComponents.at(typeName.c_str());
			storage.StringDeserializer(guid, registry, data, entityHierarchyIndex);
			return;
		}

		throw std::runtime_error("Type is not registered for deserialization");
	}

	YAML::Node Project::SerializeComponentToYaml(ECS::Registry &registry, ECS::ComponentPtr component)
	{
		const char *typeName = component.typeName;

		if (m_AvailableComponents.find(typeName) != m_AvailableComponents.end())
		{
			auto &componentStorage = m_AvailableComponents.at(typeName);
			void *obj			   = registry.GetRawComponent(component.typeName, component.componentIndex);
			return componentStorage.YamlSerializer(obj);
		}

		throw std::runtime_error("Type is not registered for serialization");
	}

	void Project::DeserializeComponentFromYaml(ECS::Registry	 &registry,
											   GUID				  guid,
											   const std::string &displayName,
											   const YAML::Node	 &node,
											   size_t			  entityHierarchyIndex)
	{
		std::string typeName = GetComponentTypeNameFromDisplayName(displayName);

		if (m_AvailableComponents.find(typeName.c_str()) != m_AvailableComponents.end())
		{
			auto &storage = m_AvailableComponents.at(typeName.c_str());
			storage.YamlDeserializer(guid, registry, node, entityHierarchyIndex);
			return;
		}
		throw std::runtime_error("Type is not registered for deserialization");
	}

	void Project::CreateComponent(const char *typeName, ECS::Registry &registry, const Entity &entity)
	{
		if (m_AvailableComponents.find(typeName) != m_AvailableComponents.end())
		{
			auto &storage = m_AvailableComponents.at(typeName);
			storage.CreationFunction(registry, entity);
			return;
		}

		throw std::runtime_error("Type is not registered for creation");
	}

	void Project::WriteProjectFile()
	{
		// create directories if they do not exist
		std::filesystem::path path = m_RootDirectory;
		std::filesystem::create_directories(path);

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << m_Name;

		out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
		for (const auto &scene : m_Scenes)
		{
			uint64_t guid = (uint64_t)scene.Guid;

			out << YAML::BeginMap;
			out << YAML::Key << "GUID" << YAML::Value << guid;
			out << YAML::Key << "Name" << YAML::Value << scene.Name;
			out << YAML::Key << "Path" << YAML::Value << scene.Path;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::Key << "StartupScene" << YAML::Value << m_StartupScene;
		out << YAML::Key << "SceneDirectory" << YAML::Value << m_SceneDirectory;
		out << YAML::Key << "AssetsDirectory" << YAML::Value << m_AssetsDirectory;
		out << YAML::Key << "ScriptsDirectory" << YAML::Value << m_ScriptsDirectory;
		out << YAML::EndMap;

		std::string filepath = m_RootDirectory + "/" + m_Name + ".proj";
		FileSystem::WriteFileAbsolute(filepath, out.c_str());
	}

	void Project::WriteSceneFile(const std::string &sceneDirectory)
	{
		// create directories if they do not exist
		std::filesystem::path path = sceneDirectory;
		std::filesystem::create_directories(path);

		if (m_LoadedScene)
		{
			std::string filename = m_LoadedScene->Name + ".scene";
			path /= filename;
			m_LoadedScene->Serialize(path.string());
		}
	}
}	 // namespace Nexus