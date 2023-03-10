#pragma once

#include "Core/nxpch.h"
#include "Scene.h"

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

            void Serialize(const std::string& directory);
            void Deserialize(const std::string& filepath);

            Scene* GetActiveScene() { return m_Scenes[m_ActiveSceneIndex]; }
            void SetActiveScene(int index) { m_ActiveSceneIndex = index; }

        private:
            std::string m_Name;
            std::vector<Scene*> m_Scenes;
            int m_ActiveSceneIndex = 0;
    };
}