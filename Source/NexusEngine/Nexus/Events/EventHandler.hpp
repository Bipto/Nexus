#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string>

#include "Event.hpp"

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
            for (auto delegate : m_EventFunctions)
                delegate.Call(param);
        }

        /// @brief A method to bind a new function to the event handler
        /// @param function The function to bind to the event handler
        void Bind(std::function<void(T)> function)
        {
            m_EventFunctions.push_back(function);
        }

        /// @brief A method that removes a function from the event handler
        /// @param function The function to unbind from the event handler
        void Unbind(std::function<void(T)> function)
        {
            auto position = std::find(m_EventFunctions.begin(), m_EventFunctions.end(), function);
            if (position != m_EventFunctions.end())
                m_EventFunctions.erase(position);
        }

        /// @brief A custom operator that can be used to bind a function to the event handler
        /// @param function The function to bind to the event handler
        /// @return The new event handler with the function bound
        EventHandler &operator+=(std::function<void(T)> function)
        {
            Bind(function);
        }

        /// @brief A custom operator that can be used to unbind a function from the event handler
        /// @param function The functio nto unbind from the event handler
        /// @return The new event handler with the function unbound
        EventHandler &operator-=(std::function<void(T)> function)
        {
            Unbind(function);
        }

        /// @brief A method that returns the amount of delegates bound to the event handler
        /// @return The amount of delegates bound to the event handler
        int GetDelegateCount() { return m_EventFunctions.size(); }

    private:
        /// @brief A vector containing the bound delegates to call
        std::vector<std::function<void(T)>> m_EventFunctions;
    };
}