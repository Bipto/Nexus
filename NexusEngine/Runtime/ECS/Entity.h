#pragma once

#include <string>
#include <list>

#include "Core/Memory.h"
#include "Components.h"

namespace Nexus
{
    class Entity
    {
    public:
        Entity(const std::string &name, int id)
            : m_Name(name), m_EntityID(id)
        {
        }

        const int GetID() { return m_EntityID; }
        bool IsActive() { return m_Active; }

        const std::string &GetName() const { return m_Name; }
        void SetName(const std::string &name) { m_Name = name; }

        const std::list<Component *> &GetComponents() { return m_Components; }
        void AddComponent(Component *component) { m_Components.push_back(component); }
        void RemoveComponent(Component *component)
        {
            m_Components.remove(component);
            delete component;
        }

        template <typename T>
        bool HasComponent()
        {
            for (auto component : m_Components)
            {
                // we need to dereference pointer to find base type
                if (typeid(T) == typeid(*component))
                {
                    return true;
                }
            }

            return false;
        }

        template <typename T>
        T *GetComponent()
        {
            for (auto component : m_Components)
            {
                /* auto tType = std::remove_pointer<T>::type();

                if (typeid(tType) == typeid(*component))
                {
                    return (T *)component;
                } */

                if (typeid(T) == typeid(*component))
                {
                    return (T *)component;
                }
            }

            return nullptr;
        }

    private:
        int m_EntityID = 0;
        std::string m_Name = "Entity";
        bool m_Active = true;
        std::list<Component *> m_Components;
    };
}