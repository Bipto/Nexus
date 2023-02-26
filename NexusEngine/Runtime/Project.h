#pragma once

#include "Core/nxpch.h"
#include "Scene.h"

#include "nlohmann/json.hpp"
#include <filesystem>

namespace Nexus
{
    class Project
    {
        public:
            Project() {}
            Project(const std::string& name) 
                : m_Name(name)
            {
                Scene* scene = new Scene("Scene");
                m_Scenes.push_back(scene);
            }

            const std::vector<Scene*> GetScenes() { return m_Scenes; }
            const std::string& GetName() { return m_Name; }

            void Serialize(const std::string& directory)
            {
                std::string baseDirectory = directory + std::string("\\") + m_Name;

                std::filesystem::path projectDirectory{baseDirectory};
                projectDirectory /= m_Name + std::string(".proj");
                std::filesystem::create_directories(projectDirectory.parent_path());

                std::filesystem::path assetDirectory{baseDirectory + std::string("\\Assets")};
                std::filesystem::create_directories(assetDirectory);

                std::filesystem::path sceneDirectory(baseDirectory + std::string("\\Scenes"));
                std::filesystem::create_directories(sceneDirectory);

                //creating json
                {
                    std::ofstream ofs(projectDirectory);

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
                for (auto scene : m_Scenes)
                {
                    std::ofstream ofs(baseDirectory + std::string("\\Scenes") + std::string("\\") + scene->GetName() + std::string(".scene"));
                    ofs << scene->GetName();
                    ofs.close();
                }
            }

            void Deserialize(const std::string& filepath)
            {

            }

        private:
            std::string m_Name;
            std::vector<Scene*> m_Scenes;
    };
}