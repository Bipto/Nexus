#pragma once

#include "Components.h"

namespace Nexus
{
    class ComponentRegistry
    {
        public:
            void Bind(Component* component);
            Component* Get(const std::string& name);

        private:
            std::unordered_map<std::string, Component*> m_Components;
    };
}