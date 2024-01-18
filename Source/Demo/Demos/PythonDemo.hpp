#pragma once

#include "Demo.hpp"

#include "pocketpy.h"

namespace Demos
{
    class PythonDemo : public Demo
    {
    public:
        PythonDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            m_CommandList = m_GraphicsDevice->CreateCommandList();
        }

        virtual ~PythonDemo()
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

        virtual void OnResize(Nexus::Point<uint32_t> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::Separator();
            ImGui::InputTextMultiline("Python Script: ", &m_PythonScript);
            if (ImGui::Button("Execute"))
            {
                pkpy::VM *vm = new pkpy::VM();
                vm->exec(m_PythonScript);
                delete vm;
            }
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

        std::string m_PythonScript = "print('Hello from Python')";
    };
}