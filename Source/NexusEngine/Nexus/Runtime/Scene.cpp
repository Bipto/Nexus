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
        YAML::Node sceneNode = root["scene"];
        sceneNode["name"] = m_Name;

        sceneNode["clear-colour"]["r"] = m_ClearColour.r;
        sceneNode["clear-colour"]["g"] = m_ClearColour.g;
        sceneNode["clear-colour"]["b"] = m_ClearColour.b;
        sceneNode["clear-colour"]["a"] = m_ClearColour.a;

        sceneNode["entities"] = m_Entities;

        YAML::Emitter emitter;
        emitter << root;

        std::string output = emitter.c_str();

        FileSystem::WriteFileAbsolute(filepath, output);
    }

    void Scene::AddEmptyEntity()
    {
        m_Entities.push_back(Entity());
    }

    const std::vector<Entity> &Scene::GetEntities() const
    {
        return m_Entities;
    }

    Scene *Scene::Deserialize(const std::string &filepath)
    {
        std::string input = Nexus::FileSystem::ReadFileToStringAbsolute(filepath);
        YAML::Node node = YAML::Load(input);
        YAML::Node sceneNode = node["scene"];

        Scene *scene = new Scene();
        scene->m_Name = sceneNode["name"].as<std::string>();
        scene->m_ClearColour.r = sceneNode["clear-colour"]["r"].as<float>();
        scene->m_ClearColour.g = sceneNode["clear-colour"]["g"].as<float>();
        scene->m_ClearColour.b = sceneNode["clear-colour"]["b"].as<float>();
        scene->m_ClearColour.a = sceneNode["clear-colour"]["a"].as<float>();
        scene->m_Entities = sceneNode["entities"].as<std::vector<Entity>>();

        return scene;
    }
}