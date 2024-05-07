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

        GetPrimaryWindow()->OnFileDrop += [&](std::string file)
        {
            m_Layout.LoadProject(file);
        };
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->BeginFrame();
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

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