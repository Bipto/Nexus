#pragma once

#include "UI/Layout.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "Nexus/Runtime/Project.hpp"

class EditorApplication : public Nexus::Application
{
public:
    EditorApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
    }

    virtual void Load() override
    {
        m_CommandList = m_GraphicsDevice->CreateCommandList();
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->BeginFrame();
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

        m_CommandList->Begin();
        m_CommandList->SetRenderTarget({m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
        m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        m_Layout.Render(time);

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point<uint32_t> size) override
    {
    }

    void CreatePipeline()
    {
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;
    Editor::Layout m_Layout{this};
};