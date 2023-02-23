#pragma once

#include "Core/nxpch.h"

namespace Nexus
{
    class Scene
    {
        public:
            Scene(const std::string& name);
        private:
            std::string m_Name;
    };
}