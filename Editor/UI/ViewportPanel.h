#include "Panel.h"

class ViewportPanel : public Panel
{
    public:
        ViewportPanel(Nexus::Ref<Nexus::Framebuffer> framebuffer)
        {
            m_Framebuffer = framebuffer;
        }

        virtual void OnRender() override
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
            ImGui::Begin("Viewport");
            auto availSize = ImGui::GetContentRegionAvail();
            if (m_Framebuffer->HasColorTexture())
                ImGui::Image((ImTextureID)m_Framebuffer->GetColorAttachment(), availSize, ImVec2(0, 1), ImVec2(1, 0));


            m_FramebufferRequiresResize = 
                (availSize.x != m_PreviousWindowSize.x ||
                availSize.y != m_PreviousWindowSize.y);

            m_PreviousWindowSize = availSize;
            ImGui::End();
            ImGui::PopStyleVar();
        }

        ImVec2 GetWindowSize() { return m_PreviousWindowSize; }
        bool FramebufferRequiresResize() { return m_FramebufferRequiresResize; }
    
    private:
        ImVec2 m_PreviousWindowSize;
        Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;
        bool m_FramebufferRequiresResize = true;
};