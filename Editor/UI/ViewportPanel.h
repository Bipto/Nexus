#include "Panel.h"

class ViewportPanel : public Panel
{
    public:
        ViewportPanel(Nexus::Framebuffer* framebuffer)
            : m_Framebuffer(framebuffer)
            {}

        virtual void OnRender() override
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
            ImGui::Begin("Viewport", &m_WindowOpen);
            auto availSize = ImGui::GetContentRegionAvail();
            if (m_Framebuffer->HasColorTexture())
                ImGui::Image((void*)m_Framebuffer->GetColorAttachment(0), availSize);
            ImGui::End();
            ImGui::PopStyleVar();
        }
    
    private:
        Nexus::Framebuffer* m_Framebuffer;
        bool m_WindowOpen = true;
};