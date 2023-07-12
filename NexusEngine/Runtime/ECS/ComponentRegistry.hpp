#pragma once

#include "Components.hpp"

namespace Nexus
{
    class ComponentRegistry
    {
    public:
        void Bind(Component *component);
        Component *Get(const std::string &name);
        std::unordered_map<std::string, Component *> &GetComponents();

    private:
        std::unordered_map<std::string, Component *> m_Components;
    };
}