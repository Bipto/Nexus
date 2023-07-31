#pragma once

#include "Core/nxpch.hpp"
#include "Scene.hpp"
#include "Core/Memory.hpp"

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

            const std::vector<std::string> GetScenes() { return m_Scenes; }

            const std::string& GetName() { return m_Name; }
            void SetName(const std::string& name) { m_Name = name; }

            void Serialize(const std::string& directory);
            static Ref<Project> Deserialize(const std::string& filepath);

            Scene* GetActiveScene() { return m_ActiveScene; }

            void SetActiveScene(int index) 
            { 
                auto filepath = m_Scenes[index];
            }

            void AddScene(const std::string& name) 
            {
                Scene* scene = new Scene(name);
                AddScene(scene);
            }

            void AddScene(Scene* scene)
            {
                m_ActiveScene = scene;
                m_Scenes.push_back(scene->GetName());
            }

            const std::string& GetProjectDirectory() { return m_ProjectDirectory; }

        private:
            std::string m_Name = {};
            std::string m_ProjectDirectory = {};

            int m_ActiveSceneIndex = 0;
            std::vector<std::string> m_Scenes;
            Scene* m_ActiveScene;
            std::string m_SceneDirectory = {};
            std::string m_AssetsDirectory = {};
    };
}