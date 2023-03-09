#include "Panel.h"

class SceneHierarchyPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Scene Hierachy");
            ImGui::Text("Hello");
            ImGui::End();
        }
};