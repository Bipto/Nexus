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
        Project(const std::string &name, const std::string &projectDirectory);
        void Serialize(const std::string &filepath);
        const std::string &GetProjectDirectory() { return m_ProjectDirectory; }

    private:
        std::string m_Name = {};
        std::string m_ProjectDirectory = {};
        std::string m_SceneDirectory = {};
        std::string m_AssetsDirectory = {};

        std::vector<std::string> m_SceneNames;
        std::unique_ptr<Scene> m_LoadedScene;
        uint32_t m_StartupScene = 0;
    };
}