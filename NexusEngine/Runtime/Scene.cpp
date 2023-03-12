#include "Scene.h"

#include "nlohmann/json.hpp"
#include <filesystem>

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
            
            for (auto entity : m_Entities)
            {
                std::stringstream ss;
                ss << entity.GetID();
                j["entities"][ss.str()] =
                {
                    { "name", entity.GetName()},
                    { "is_active", entity.IsActive()}
                };
            }

            ofs << j.dump(1);
            ofs.close();
        }
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
            
            for (int i = 0; i < entityCount; i++)
            {
                std::stringstream ss;
                ss << i;
                std::string id = ss.str();
                auto entityName = j["entities"][id]["name"];
                auto entityIsActive = j["entities"][id]["is_active"];

                Entity entity(entityName, i);
                m_Entities.push_back(entity);
            }
        }
    }
}