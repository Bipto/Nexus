#include "ComponentRegistry.h"

namespace Nexus
{
    void ComponentRegistry::Bind(Component* component)
    {
        m_Components[component->GetName()] = component;
    }

    Component* ComponentRegistry::Get(const std::string& name)
    {   
        if (m_Components.find(name) != m_Components.end())
            return m_Components[name];
        return {};
    }
}