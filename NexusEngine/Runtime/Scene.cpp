#include "Scene.h"

#include "nlohmann/json.hpp"
#include <filesystem>
#include <iostream>

#include "ECS/ComponentRegistry.h"
#include "Core/Runtime.h"

namespace Nexus
{
    void Scene::Serialize(const std::string& sceneDirectory)
    {
        std::filesystem::path scenePath(sceneDirectory + std::string("\\") + m_Name + std::string(".scene"));
        std::ofstream ofs(scenePath);

        //creating json
        {
            nlohmann::json j;
            j["name"] = m_Name;
            j["entity_count"] = m_Entities.size();
            j["clearColor"] = 
            {
                { "r", m_ClearColor.r },
                { "g", m_ClearColor.g },
                { "b", m_ClearColor.b },
                { "a", m_ClearColor.a }
            };
            
            //serialize entities
            for (auto entity : m_Entities)
            {
                nlohmann::json componentJson;
                int index = 0;
                for (auto component : entity.GetComponents())
                {
                    /* componentJson[std::to_string(index)] = 
                    {
                        { "name", component->GetName() },
                        { "data", component->Serialize() }
                    }; */

                    componentJson[std::to_string(index)] = 
                    {
                        { "name", component->GetName() },
                        { "data", component->Serialize() }
                    };

                    index++;
                }

                //add entity json to scene
                std::stringstream entityJson;
                entityJson << entity.GetID();
                j["entities"][entityJson.str()] =
                {
                    { "name", entity.GetName() },
                    { "is_active", entity.IsActive() },
                    { "component_count", entity.GetComponents().size() },
                    { "components", componentJson }
                };
            }

            ofs << j.dump(1);
            ofs.close();
        }
    }

    void Scene::LoadComponent(Entity& entity, nlohmann::json json)
    {
        std::string name = json["name"];
        auto componentData = json["data"];

        auto& registry = GetComponentRegistry();
        auto component = registry.Get(name);
        auto newComponent = component->Clone();
        newComponent->Deserialize(componentData);
        entity.AddComponent(newComponent);
    }

    void Scene::LoadEntity(nlohmann::json json, int id)
    {
        auto entityName = json["name"];
        auto  entityIsActive = json["is_active"];

        Entity entity(entityName, id);

        int componentCount = json["component_count"];

        for (int i = 0; i < componentCount; i++)
        {
            auto componentJson = json["components"][std::to_string(i)];
            LoadComponent(entity, componentJson);
        }

        m_Entities.push_back(entity);
    }
    
    void Scene::Deserialize(const std::string& filepath)
    {
        std::filesystem::path path(filepath);
        std::ifstream file(path);
        std::string line;
        std::stringstream ss;

        if (file.is_open())
            while (getline(file, line))
                ss << line;
                
        //load json
        {
            nlohmann::json j = nlohmann::json::parse(ss.str());
            m_Name = j["name"];
            int entityCount = j["entity_count"].get<int>();

            m_ClearColor.r = j["clearColor"]["r"].get<float>();
            m_ClearColor.g = j["clearColor"]["g"].get<float>();
            m_ClearColor.b = j["clearColor"]["b"].get<float>();
            m_ClearColor.a = j["clearColor"]["a"].get<float>();
            
            for (int i = 0; i < entityCount; i++)
            {
                std::string id = std::to_string(i);
                auto entityJson = j["entities"][id];
                LoadEntity(entityJson, std::stoi(id));
            }
        }
    }
}