#include "Project.h"

#include "nlohmann/json.hpp"

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
            j["scene_directory"] = m_SceneDirectory;
            j["assets_directory"] = m_AssetsDirectory;
            std::vector<std::string> paths;
            for (auto scene : m_Scenes)
                paths.emplace_back(scene->GetName());
            j["scenes"] = paths;

            //write to file
            ofs << j.dump(1);
            ofs.close();
        }

        //create scene subdirectories
        std::string sceneDirectory = baseDirectory + "\\Scenes";
        for (auto scene : m_Scenes)         
            scene->Serialize(sceneDirectory);
    }

    Ref<Project> Project::Deserialize(const std::string &filepath)
    {
        std::stringstream ss;
        std::string line;
        std::ifstream file(filepath);
        auto projectDirectory = std::filesystem::path(filepath).parent_path();

        //read lines into buffer
        if (file.is_open())
            while (getline(file, line))
                ss << line;

        
        nlohmann::json j = nlohmann::json::parse(ss.str());

        auto name = j.value("name", "not found");
        Ref<Project> project = CreateRef<Project>(name, filepath);

        std::string assetDir = j["assets_directory"].get<std::string>();
        project->m_AssetsDirectory = assetDir;
        
        std::string sceneDir = j["scene_directory"];
        project->m_SceneDirectory = sceneDir;

        std::vector<std::string> sceneNames;
        sceneNames = j["scenes"];

        std::filesystem::path sceneDirectory(sceneDir);
        std::vector<Scene*> scenes;
        for (auto sceneName : sceneNames)
        {
            Scene* scene = new Scene(sceneName);
            std::string scenePath(projectDirectory.string() + std::string("\\") + sceneDir + std::string("\\") + sceneName + std::string(".scene"));
            scene->Deserialize(scenePath);
            scenes.push_back(scene);
        }      
        project->m_Scenes = scenes;      

        return project;
    }
}