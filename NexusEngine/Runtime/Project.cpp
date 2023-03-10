#include "Project.h"

#include "nlohmann/json.hpp"
#include <filesystem>

namespace Nexus
{
    void Project::Serialize(const std::string &directory)
    {
        std::string baseDirectory = directory + std::string("\\") + m_Name;

        std::filesystem::path projectFile{baseDirectory};
        projectFile /= m_Name + std::string(".proj");
        std::filesystem::create_directories(projectFile.parent_path());

        std::filesystem::path assetDirectory{baseDirectory + std::string("\\Assets")};
        std::filesystem::create_directories(assetDirectory);

        std::filesystem::create_directories(baseDirectory + std::string("\\Scenes"));

        //creating json
        {
            std::ofstream ofs(projectFile);

            nlohmann::json j;
            j["name"] = m_Name;
            std::vector<std::string> paths;
            for (auto scene : m_Scenes)
                paths.emplace_back(scene->GetName());
            j["scenes"] = paths;

            ofs << j.dump(1);
            ofs.close();
        }

        //create scene subdirectories
        std::string sceneDirectory = baseDirectory + std::string("\\Scenes");
        for (auto scene : m_Scenes)
        {
            /* std::ofstream ofs(baseDirectory + std::string("\\Scenes") + std::string("\\") + scene->GetName() + std::string(".scene"));
            ofs << scene->GetName();
            ofs.close(); */

            
            scene->Serialize(sceneDirectory);
        }
    }
    void Project::Deserialize(const std::string &filepath)
    {
    }
}