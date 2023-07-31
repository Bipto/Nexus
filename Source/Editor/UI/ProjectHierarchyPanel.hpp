#include "Panel.hpp"

class ProjectHierarchyPanel : public Panel
{
public:
    virtual void OnRender() override
    {
        ImGui::Begin("Project Hierarchy");

        if (m_Project)
        {
            for (auto scene : m_Project->GetScenes())
            {
                auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;
                auto text = scene;

                if (ImGui::TreeNodeEx(text.c_str(), flags))
                {
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }
};