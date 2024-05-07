#include "SceneViewport.hpp"

#include "Nexus/Runtime/Project.hpp"

namespace Editor
{
    SceneViewport::SceneViewport(Nexus::Graphics::GraphicsDevice *graphicsDevice, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiGraphicsRenderer)
        : m_GraphicsDevice(graphicsDevice), m_ImGuiGraphicsRenderer(imGuiGraphicsRenderer)
    {
        m_CommandList = m_GraphicsDevice->CreateCommandList();
    }

    void SceneViewport::OnLoad()
    {
    }

    void SceneViewport::OnRender()
    {
        if (m_Enabled)
        {
            if (ImGui::Begin(SCENE_VIEWPORT_NAME, &m_Enabled))
            {
                ImVec2 size = ImGui::GetContentRegionAvail();
                if (size.x != m_CurrentWidth || size.y != m_CurrentHeight || !m_ViewportFramebuffer)
                {
                    ResizeViewport((uint32_t)size.x, (uint32_t)size.y);

                    if (Nexus::Project::s_ActiveProject)
                    {
                        RenderScene();
                        ImGui::Image(m_ViewportTextureID, {size.x, size.y});
                    }
                }
            }

            ImGui::End();
        }
    }

    void SceneViewport::ResizeViewport(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        if (m_ViewportFramebuffer)
        {
            m_ImGuiGraphicsRenderer->UnbindTexture(m_ViewportTextureID);
        }

        Nexus::Graphics::FramebufferSpecification spec;
        spec.Width = width;
        spec.Height = height;
        spec.ColorAttachmentSpecification.Attachments = {{Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm}};
        spec.DepthAttachmentSpecification.DepthFormat = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
        spec.Samples = Nexus::Graphics::SampleCount::SampleCount1;

        m_ViewportFramebuffer = m_GraphicsDevice->CreateFramebuffer(spec);
        m_ViewportTextureID = m_ImGuiGraphicsRenderer->BindTexture(m_ViewportFramebuffer->GetColorTexture(0));
    }

    void SceneViewport::RenderScene()
    {
        Nexus::Ref<Nexus::Project> project = Nexus::Project::s_ActiveProject;

        if (!project)
        {
            return;
        }

        Nexus::Scene *scene = project->GetLoadedScene();

        if (!scene)
        {
            return;
        }

        const glm::vec4 &clearColour = scene->GetClearColour();

        m_CommandList->Begin();
        m_CommandList->SetRenderTarget({m_ViewportFramebuffer});
        m_CommandList->ClearColorTarget(0, {clearColour.r,
                                            clearColour.g,
                                            clearColour.b,
                                            clearColour.a});
        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);
    }
}