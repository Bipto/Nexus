#include "Panel.h"

class InspectorPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            ImGui::Begin("Inspector");
            ImGui::Text("Hello");
            ImGui::End();
        }
};