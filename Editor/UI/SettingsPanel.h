#include "Panel.h"

class SettingsPanel : public Panel
{
    public:
        SettingsPanel(float* movementSpeed, glm::vec3* quadPosition, glm::vec3* quadSize)
            : m_MovementSpeed(movementSpeed), m_QuadPosition((float*)quadPosition), m_QuadSize((float*)quadSize)
            {}

        virtual void OnRender() override
        {
            ImGui::Begin("Settings");
            ImGui::Text("Camera");
            ImGui::DragFloat("Movement Speed", m_MovementSpeed, 0.1f, 0.1f, 5.0f);

            ImGui::Separator();
            ImGui::Text("Quad");
            ImGui::DragFloat2("Quad Position", m_QuadPosition, 0.1f);
            ImGui::DragFloat2("Quad Size", m_QuadSize);

            ImGui::End();
        }
    private:
        float* m_MovementSpeed;
        float* m_QuadPosition;
        float* m_QuadSize;
};