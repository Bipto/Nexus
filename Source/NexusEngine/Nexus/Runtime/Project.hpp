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
        Project(const std::string &name = "Untitled Project", const std::string &directory = "");
        void Serialize();
        static Ref<Project> Deserialize(const std::string &filepath);

        Scene *GetLoadedScene() { return m_LoadedScene.get(); }

        const std::string &GetName() const { return m_Name; }
        void SetName(const std::string &name) { m_Name = name; }

        void LoadScene(uint32_t index);

    public:
        static Ref<Project> s_ActiveProject;

    private:
        void WriteProjectFile();
        void WriteSceneFile(const std::string &sceneDirectory);

    private:
        std::string m_Name = {};
        std::string m_RootDirectory = {};
        std::string m_SceneDirectory = {};
        std::string m_AssetsDirectory = {};

        std::vector<std::string> m_SceneNames;
        std::unique_ptr<Scene> m_LoadedScene;
        uint32_t m_StartupScene = 0;
    };
}