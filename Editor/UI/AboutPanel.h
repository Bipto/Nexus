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
            ImGui::Text(APIToString());
            ImGui::End();
        }
    private:
        const char* APIToString()
        {
                switch (m_GraphicsDevice->GetGraphicsAPI())
                {
                    default:
                        return "Graphics API: OpenGL 3.3";
                }
        }

        Nexus::Ref<Nexus::GraphicsDevice> m_GraphicsDevice;
};