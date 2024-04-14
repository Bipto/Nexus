#include "Project.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "yaml-cpp/yaml.h"

#include <ctime>
#include <iomanip>

const std::string DefaultSceneName = "UntitledScene";

namespace Nexus
{
    Ref<Project> Project::s_ActiveProject = nullptr;

    Project::Project(const std::string &name)
        : m_Name(name)
    {
        m_SceneDirectory = "\\Scenes";
        m_AssetsDirectory = "\\Assets";

        m_LoadedScene = std::make_unique<Scene>(DefaultSceneName);
        m_SceneNames.push_back(m_LoadedScene->GetName());
    }

    void Project::Serialize(const std::string &filepath)
    {
        // std::filesystem::path projectFile(filepath);
        // std::string projectDirectory = projectFile.parent_path().parent_path().string();#

        std::filesystem::path path = filepath;
        std::filesystem::path root = path.parent_path();

        WriteProjectFile(filepath);
        WriteSceneFile(root.string(), m_SceneDirectory);
    }

    Ref<Project> Project::Deserialize(const std::string &filepath)
    {
        Ref<Project> project = CreateRef<Project>("");
        return project;
    }

    void Project::WriteProjectFile(const std::string &filepath)
    {
        // create directories if they do not exist
        std::filesystem::path path = filepath;
        std::filesystem::create_directories(path.parent_path());

        YAML::Node root;
        root["name"] = m_Name;

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
        root["time-created"] = timestampString;
        root["time-updated"] = timestampString;

        root["scenes"] = m_SceneNames;
        root["startup-scene"] = m_StartupScene;

        root["scene-directory"] = m_SceneDirectory;
        root["assets-directory"] = m_AssetsDirectory;

        std::string timestring;

        YAML::Emitter emitter;
        emitter << root;

        std::string output = emitter.c_str();

        FileSystem::WriteFileAbsolute(filepath, output);
    }

    void Project::WriteSceneFile(const std::string &rootpath, const std::string &sceneDirectory)
    {
        // create directories if they do not exist
        std::filesystem::path path = rootpath + sceneDirectory;
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