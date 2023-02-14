#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string>

#include "Event.h"

template <typename T>
class Delegate
{
    public:
        Delegate(const std::string &name, std::function<void(T param)> function)
        {
            m_Name = name;
            m_Function = function;
        }

        bool operator == (const Delegate& delegate)
        {
            if (m_Name == delegate.m_Name)
            {
                return true;
            }
            return false;
        }

        void Call(T param)
        {
            m_Function(param);
        }

        const std::string& GetName() { return m_Name; }

    private:
        std::string m_Name;
        std::function<void(T param)> m_Function;
};

namespace Nexus
{
    template <typename T>
    class EventHandler
    {
        public:
            void Invoke(T param)
            {
                for (auto delegate : m_Delegates)
                    delegate.Call(param);
            }

            void Bind(Delegate<T> function)
            {
                m_Delegates.push_back(function);
            }

            void Unbind(Delegate<T> function)
            {
                auto position = std::find(m_Delegates.begin(), m_Delegates.end(), function);
                if (position != m_Delegates.end())
                    m_Delegates.erase(position);
            }

            EventHandler& operator +=(Delegate<T> function)
            {
                Bind(function);
            }

            EventHandler& operator -=(Delegate<T> function)
            {
                Unbind(function);
            }

            int GetDelegateCount() { return m_Delegates.size(); }

        private:
            std::vector<Delegate<T>> m_Delegates;
    };
}