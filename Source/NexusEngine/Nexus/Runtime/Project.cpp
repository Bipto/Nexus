#include "Project.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "yaml-cpp/yaml.h"

#include <ctime>
#include <iomanip>

const std::string DefaultSceneName = "Unnamed.scene";

namespace Nexus
{
    Project::Project(const std::string &name, const std::string &projectDirectory)
        : m_Name(name), m_ProjectDirectory(projectDirectory)
    {
        m_SceneDirectory = "\\Scenes";
        m_AssetsDirectory = "\\Assets";

        std::filesystem::path projectFile(projectDirectory);
        m_ProjectDirectory = projectFile.parent_path().parent_path().string();

        m_LoadedScene = std::make_unique<Scene>(DefaultSceneName);
        m_SceneNames.push_back(m_LoadedScene->GetName());
    }

    void Project::Serialize(const std::string &filepath)
    {
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

        std::string timestring;

        YAML::Emitter emitter;
        emitter << root;

        std::string output = emitter.c_str();

        FileSystem::WriteFileAbsolute(filepath, output);
    }
}