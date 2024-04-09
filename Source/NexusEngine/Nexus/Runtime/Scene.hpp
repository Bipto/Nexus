#pragma once

#include <string>

namespace Nexus
{
    class Scene
    {
    public:
        Scene() = delete;
        Scene(const std::string &name);

        const std::string &GetName() { return m_Name; }

    private:
        std::string m_Name;
    };
}