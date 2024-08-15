#include "Project.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "yaml-cpp/yaml.h"

const std::string DefaultSceneName = "UntitledScene";

namespace Nexus
{
Ref<Project> Project::s_ActiveProject = nullptr;

Project::Project(const std::string &name, const std::string &directory) : m_Name(name), m_RootDirectory(directory)
{
    m_SceneDirectory = "\\Scenes";
    m_AssetsDirectory = "\\Assets";

    CreateNewScene(DefaultSceneName);
}

void Project::Serialize()
{
    WriteProjectFile();
    WriteSceneFile(m_RootDirectory + "/" + m_SceneDirectory);
}

Ref<Project> Project::Deserialize(const std::string &filepath)
{
    std::string directory = std::filesystem::path(filepath).parent_path().string();

    std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
    YAML::Node node = YAML::Load(input);

    Ref<Project> project = CreateRef<Project>();
    project->m_Name = node["Project"].as<std::string>();
    project->m_RootDirectory = directory;

    auto scenes = node["Scenes"];
    if (scenes)
    {
        for (auto scene : scenes)
        {
            SceneInfo sceneInfo;
            sceneInfo.Name = scene["Name"].as<std::string>();
            sceneInfo.Path = scene["Path"].as<std::string>();
            project->m_Scenes.push_back(sceneInfo);
        }
    }

    project->m_StartupScene = node["StartupScene"].as<uint32_t>();
    project->m_SceneDirectory = node["SceneDirectory"].as<std::string>();
    project->m_AssetsDirectory = node["AssetsDirectory"].as<std::string>();

    project->LoadScene(project->m_StartupScene);

    return project;
}

void Project::LoadScene(uint32_t index)
{
    if (m_Scenes.size() > index)
    {
        const SceneInfo &info = m_Scenes.at(index);
        std::string path = m_RootDirectory + info.Path;
        Scene *scene = Scene::Deserialize(path);
        m_LoadedScene = std::unique_ptr<Scene>(scene);
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
    info.Name = name;
    info.Path = path;
    m_Scenes.push_back(info);

    m_LoadedScene = std::make_unique<Scene>(name);
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
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << scene.Name;
        out << YAML::Key << "Path" << YAML::Value << scene.Path;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    out << YAML::Key << "StartupScene" << YAML::Value << m_StartupScene;
    out << YAML::Key << "SceneDirectory" << YAML::Value << m_SceneDirectory;
    out << YAML::Key << "AssetsDirectory" << YAML::Value << m_AssetsDirectory;
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
        std::string filename = m_LoadedScene->GetName() + ".scene";
        path /= filename;
        m_LoadedScene->Serialize(path.string());
    }
}
} // namespace Nexus