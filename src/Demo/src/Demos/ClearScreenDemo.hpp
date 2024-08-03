#pragma once

#include "Demo.hpp"

namespace Demos
{
    class ClearScreenDemo : public Demo
    {
    public:
        ClearScreenDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
            : Demo(name, app, imGuiRenderer)
        {
        }

        virtual ~ClearScreenDemo()
        {
        }

        virtual void Load() override
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();
        }

        virtual void Render(Nexus::Time time) override
        {
            m_CommandList->Begin();
            m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget{m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            m_CommandList->ClearColorTarget(0, {m_ClearColour.r,
                                                m_ClearColour.g,
                                                m_ClearColour.b,
                                                1.0f});
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void RenderUI() override
        {
            ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}