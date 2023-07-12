#pragma once

#include "NexusEngine.hpp"

#include "imgui.h"

class Panel
{
public:
    virtual void OnLoad() {}
    virtual void OnUnload() {}
    virtual void OnRender() {}

    void Enable() { m_Enabled = true; }
    void Disable() { m_Enabled = false; }
    bool IsEnabled() { return m_Enabled; }

    void LoadProject(Nexus::Ref<Nexus::Project> project)
    {
        m_Project = project;
    }

    void UnloadProject()
    {
        m_Project = nullptr;
    }

    void SetActiveEntity(int entityID)
    {
        m_SelectedEntityID = entityID;
    }

    void ClearSelectedEntity()
    {
        m_SelectedEntityID = -1;
    }

protected:
    Nexus::Ref<Nexus::Project> m_Project;
    int m_SelectedEntityID = -1;
    bool m_Enabled = true;
};