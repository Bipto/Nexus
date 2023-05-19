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

            ImGui::Text("Graphics API:");
            if (m_GraphicsDevice->GetGraphicsAPI() == Nexus::GraphicsAPI::OpenGL)
            {
                ImGui::Text("OpenGL Version ");
                ImGui::SameLine();
            }
            ImGui::Text("%s", m_GraphicsDevice->GetAPIName());
            ImGui::Separator();

            ImGui::Text("Graphics Adapter:");
            ImGui::Text("%s", m_GraphicsDevice->GetDeviceName());
            ImGui::Separator();
            ImGui::End();       
        }
    private:
        Nexus::Ref<Nexus::GraphicsDevice> m_GraphicsDevice;
};