#pragma once

#include "Nexus/nxpch.hpp"
#include "Nexus/Types.hpp"

#include "Scene.hpp"

#include <filesystem>

namespace Nexus
{
    class Project
    {
    public:
        Project(const std::string &name);
        void Serialize(const std::string &filepath);
        static Ref<Project> Deserialize(const std::string &filepath);

        Scene *GetLoadedScene() { return m_LoadedScene.get(); }

    public:
        static Ref<Project> s_ActiveProject;

    private:
        void WriteProjectFile(const std::string &filepath);
        void WriteSceneFile(const std::string &rootpath, const std::string &sceneDirectory);

    private:
        std::string m_Name = {};
        std::string m_SceneDirectory = {};
        std::string m_AssetsDirectory = {};

        std::vector<std::string> m_SceneNames;
        std::unique_ptr<Scene> m_LoadedScene;
        uint32_t m_StartupScene = 0;
    };
}