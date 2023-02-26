#pragma once

#include "Core/nxpch.h"
#include "Scene.h"

namespace Nexus
{
    class Project
    {
        public:
            Project(const std::string& name) 
                : m_Name(name)
            {
                
            }

            const std::string& GetName()
            {
                return m_Name;
            }

        private:
            std::string m_Name;
            std::vector<Scene> m_Scenes;
    };
}