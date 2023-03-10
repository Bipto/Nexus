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
            
            for (auto entity : m_Entities)
            {
                j["entities"][entity.GetID()] =
                {
                    { "name", entity.GetName()},
                    { "is_active", entity.IsActive()}
                };
            }

            ofs << j.dump(1);
            ofs.close();
        }
    }
    
    void Scene::Deserialize(const std::string filepath)
    {

    }
}