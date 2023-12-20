#pragma once

#include "Demo.hpp"

namespace Demos
{
    class ClearScreenDemo : public Demo
    {
    public:
        ClearScreenDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();
        }

        virtual ~ClearScreenDemo()
        {
            delete m_CommandList;
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            m_CommandList->Begin();
            m_CommandList->SetRenderTarget({m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            m_CommandList->ClearColorTarget(0, {m_ClearColour.r,
                                                m_ClearColour.g,
                                                m_ClearColour.b,
                                                1.0f});
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};
    };
}