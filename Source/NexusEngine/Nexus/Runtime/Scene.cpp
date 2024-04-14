#include "Scene.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "yaml-cpp/yaml.h"

namespace Nexus
{
    Scene::Scene(const std::string &name)
        : m_Name(name)
    {
    }

    void Scene::Serialize(const std::string &filepath)
    {
        YAML::Node root;
        root["name"] = m_Name;

        root["clear-colour"]["r"] = m_ClearColour.r;
        root["clear-colour"]["g"] = m_ClearColour.g;
        root["clear-colour"]["b"] = m_ClearColour.b;
        root["clear-colour"]["a"] = m_ClearColour.a;

        YAML::Emitter emitter;
        emitter << root;

        std::string output = emitter.c_str();

        FileSystem::WriteFileAbsolute(filepath, output);
    }
}