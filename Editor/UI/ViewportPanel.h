#include "Panel.h"

class ViewportPanel : public Panel
{
public:
    ViewportPanel(Nexus::Ref<Nexus::Framebuffer> framebuffer, Nexus::Ref<Nexus::GraphicsDevice> graphicsDevice)
    {
        m_Framebuffer = framebuffer;
        m_GraphicsDevice = graphicsDevice;
    }

    virtual void OnRender() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Viewport");
        auto availSize = ImGui::GetContentRegionAvail();
        if (m_Framebuffer->HasColorTexture())
        {
            ImVec2 uv = ImVec2(
                0,
                1 * m_GraphicsDevice->GetUVCorrection());
            ImGui::Image((ImTextureID)m_Framebuffer->GetColorAttachment(), availSize, uv, ImVec2(1, 0));
        }

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
    Nexus::Ref<Nexus::GraphicsDevice> m_GraphicsDevice;
    bool m_FramebufferRequiresResize = true;
};