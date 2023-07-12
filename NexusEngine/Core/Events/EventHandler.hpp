#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string>

#include "Event.hpp"

/// @brief A custom wrapper around a function pointer
/// @tparam T The payload used by the event
template <typename T>
class Delegate
{
public:
    /// @brief A constructor that creates a new delegate, with a custom name and a function to call
    /// @param name A name provided to identify the event in an event handler
    /// @param function The function to call the delegate is invoked
    Delegate(const std::string &name, std::function<void(T param)> function)
    {
        m_Name = name;
        m_Function = function;
    }

    /// @brief A custom equals operator to chech if two delegates are the same
    /// @param delegate Another delegate to check equality against
    /// @return Returns true or false depending on whether the delegates match
    bool operator==(const Delegate &delegate)
    {
        if (m_Name == delegate.m_Name)
        {
            return true;
        }
        return false;
    }

    /// @brief A method that calls the function provided when the delegate was created, with a templated parameter
    /// @param param The payload to use for the event
    void Call(T param)
    {
        m_Function(param);
    }

    /// @brief A method that returns the name of the delegate
    /// @return A const reference to the name
    const std::string &GetName() { return m_Name; }

private:
    /// @brief The name of the delegate
    std::string m_Name;

    /// @brief The function to call when the delegate is invoked
    std::function<void(T param)> m_Function;
};

namespace Nexus
{
    /// @brief A class that is used to call a group of functions when an event occurs
    /// @tparam T The parameters to use when invoking the event functions
    template <typename T>
    class EventHandler
    {
    public:
        /// @brief A method that calls the functions using a templated parameter
        /// @param param The payload to use for the functions
        void Invoke(T param)
        {
            for (auto delegate : m_Delegates)
                delegate.Call(param);
        }

        /// @brief A method to bind a new function to the event handler
        /// @param function The function to bind to the event handler
        void Bind(Delegate<T> function)
        {
            m_Delegates.push_back(function);
        }

        /// @brief A method that removes a function from the event handler
        /// @param function The function to unbind from the event handler
        void Unbind(Delegate<T> function)
        {
            auto position = std::find(m_Delegates.begin(), m_Delegates.end(), function);
            if (position != m_Delegates.end())
                m_Delegates.erase(position);
        }

        /// @brief A custom operator that can be used to bind a function to the event handler
        /// @param function The function to bind to the event handler
        /// @return The new event handler with the function bound
        EventHandler &operator+=(Delegate<T> function)
        {
            Bind(function);
        }

        /// @brief A custom operator that can be used to unbind a function from the event handler
        /// @param function The functio nto unbind from the event handler
        /// @return The new event handler with the function unbound
        EventHandler &operator-=(Delegate<T> function)
        {
            Unbind(function);
        }

        /// @brief A method that returns the amount of delegates bound to the event handler
        /// @return The amount of delegates bound to the event handler
        int GetDelegateCount() { return m_Delegates.size(); }

    private:
        /// @brief A vector containing the bound delegates to call
        std::vector<Delegate<T>> m_Delegates;
    };
}