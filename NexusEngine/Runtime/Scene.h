#pragma once

#include "Core/nxpch.h"

namespace Nexus
{
    class Scene
    {
        public:
            Scene(const std::string& name)
            {
                m_Name = name;
            }

            const std::string& GetName() { return m_Name; }

        private:
            std::string m_Name;
    };
}