#pragma once

#include "NexusEngine.h"

#include "imgui.h"

class Panel
{
    public:
        virtual void OnLoad() {}
        virtual void OnUnload() {}
        virtual void OnUpdate() {}
        virtual void OnRender() {}

        void Enable() { m_Enabled = true; }
        void Disable() { m_Enabled = false; }
        bool IsEnabled() { return m_Enabled; }
    protected:
        bool m_Enabled = true;
};