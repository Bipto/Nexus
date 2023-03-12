#pragma once

#include "Core/nxpch.h"
#include "Scene.h"
#include "Core/Memory.h"

#include <filesystem>

namespace Nexus
{
    class Project
    {
        public:
            Project(const std::string& name, const std::string& projectDirectory) 
                : m_Name(name), m_ProjectDirectory(projectDirectory)
            {
                m_SceneDirectory = "\\Scenes";
                m_AssetsDirectory = "\\Assets";

                std::filesystem::path projectFile(projectDirectory);
                m_ProjectDirectory = projectFile.parent_path().parent_path().string();

                Scene* scene = new Scene("Scene");
                AddScene(scene);
            }

            const std::vector<Scene*> GetScenes() { return m_Scenes; }

            const std::string& GetName() { return m_Name; }
            void SetName(const std::string& name) { m_Name = name; }

            void Serialize(const std::string& directory);
            static Ref<Project> Deserialize(const std::string& filepath);

            Scene* GetActiveScene() { return m_Scenes[m_ActiveSceneIndex]; }
            void SetActiveScene(int index) { m_ActiveSceneIndex = index; }
            void AddScene(const std::string& name) 
            {
                Scene* scene = new Scene(name);
                m_Scenes.push_back(scene);
            }

            void AddScene(Scene* scene)
            {
                m_Scenes.push_back(scene);
            }

            const std::string& GetProjectDirectory() { return m_ProjectDirectory; }

        private:
            std::string m_Name = {};
            std::string m_ProjectDirectory = {};

            std::vector<Scene*> m_Scenes;
            int m_ActiveSceneIndex = 0;

            std::string m_SceneDirectory = {};
            std::string m_AssetsDirectory = {};
    };
}