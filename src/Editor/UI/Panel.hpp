#pragma once

#include "Defines.hpp"

#include "Nexus-Core/ImGui/ImGuiInclude.hpp"

namespace Editor
{
    class Panel
    {
    public:
        virtual void OnLoad() = 0;
        virtual void OnRender() = 0;

        void Enable() { m_Enabled = true; }
        void Disable() { m_Enabled = false; };
        bool IsEnabled() const { return m_Enabled; }

    protected:
        bool m_Enabled = false;
    };
}