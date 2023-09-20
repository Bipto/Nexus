#include "ComponentRegistry.hpp"

namespace Nexus
{
    void ComponentRegistry::Bind(Component *component)
    {
        m_Components[component->GetName()] = component;
    }

    Component *ComponentRegistry::Get(const std::string &name)
    {
        if (m_Components.find(name) != m_Components.end())
            return m_Components[name];
        return {};
    }

    std::unordered_map<std::string, Component *> &ComponentRegistry::GetComponents()
    {
        return m_Components;
    }
}