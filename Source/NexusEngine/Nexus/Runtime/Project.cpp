#include "Project.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "yaml-cpp/yaml.h"

#include <ctime>
#include <iomanip>

const std::string DefaultSceneName = "UntitledScene";

namespace Nexus
{
    Ref<Project> Project::s_ActiveProject = nullptr;

    Project::Project(const std::string &name, const std::string &directory)
        : m_Name(name), m_RootDirectory(directory)
    {
        m_SceneDirectory = "\\Scenes";
        m_AssetsDirectory = "\\Assets";

        CreateNewScene(DefaultSceneName);
    }

    void Project::Serialize()
    {
        // std::filesystem::path projectFile(filepath);
        // std::string projectDirectory = projectFile.parent_path().parent_path().string();#

        WriteProjectFile();
        WriteSceneFile(m_RootDirectory + "/" + m_SceneDirectory);
    }

    Ref<Project> Project::Deserialize(const std::string &filepath)
    {
        std::string directory = std::filesystem::path(filepath).parent_path().string();

        std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
        YAML::Node node = YAML::Load(input);
        YAML::Node projectNode = node["project"];

        Ref<Project> project = CreateRef<Project>();
        project->m_Name = projectNode["name"].as<std::string>();
        project->m_StartupScene = projectNode["startup-scene"].as<uint32_t>();
        project->m_Scenes = projectNode["scenes"].as<std::vector<SceneInfo>>();
        project->m_SceneDirectory = projectNode["scene-directory"].as<std::string>();
        project->m_AssetsDirectory = projectNode["assets-directory"].as<std::string>();
        project->m_RootDirectory = directory;

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

        YAML::Node root;
        YAML::Node projectNode = root["project"];
        projectNode["name"] = m_Name;

        time_t curr_time = time(nullptr);
        tm *tm_local = localtime(&curr_time);

        std::string timestampString;
        timestampString += std::to_string(tm_local->tm_mday) + "/";

        // months start at 0
        timestampString += std::to_string(tm_local->tm_mon + 1) + "/";

        // years start at 1900
        timestampString += std::to_string(1900 + tm_local->tm_year) + "-";

        timestampString += std::to_string(tm_local->tm_hour) + ":";
        timestampString += std::to_string(tm_local->tm_min);
        // projectNode["time-created"] = timestampString;
        // projectNode["time-updated"] = timestampString;

        projectNode["scenes"] = m_Scenes;
        projectNode["startup-scene"] = m_StartupScene;

        projectNode["scene-directory"] = m_SceneDirectory;
        projectNode["assets-directory"] = m_AssetsDirectory;

        std::string timestring;

        YAML::Emitter emitter;
        emitter << root;
        std::string output = emitter.c_str();

        std::string filepath = m_RootDirectory + "/" + m_Name + ".proj";
        FileSystem::WriteFileAbsolute(filepath, output);
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

        /* std::string filename = m_Name + ".scene";
        path /= filename;

        YAML::Node root;
        root["name"] = m_Name;

        YAML::Emitter emitter;
        emitter << root;

        std::string output = emitter.c_str();

        FileSystem::WriteFileAbsolute(path.string(), output); */
    }
}