#include "Panel.h"

class AboutPanel : public Panel
{
    public:
        AboutPanel(Nexus::Ref<Nexus::GraphicsDevice> graphicsDevice)
            : m_GraphicsDevice(graphicsDevice)
            {}

        virtual void OnRender() override
        {
            ImGui::Begin("About", &m_Enabled);
            ImGui::Text("Nexus Engine - Version 0.1 Alpha");
            ImGui::Separator();
            ImGui::Text(m_GraphicsDevice->GetAPIName());
            ImGui::Text(m_GraphicsDevice->GetDeviceName());
            ImGui::End();
        }
    private:
        Nexus::Ref<Nexus::GraphicsDevice> m_GraphicsDevice;
};