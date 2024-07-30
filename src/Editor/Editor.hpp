#pragma once

#include "UI/Layout.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Runtime/Project.hpp"

class EditorApplication : public Nexus::Application
{
public:
    explicit EditorApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
    }

    ~EditorApplication()
    {
        m_Layout.SaveLayout("layout.ini");
    }

    virtual void Load() override
    {
        m_CommandList = m_GraphicsDevice->CreateCommandList();

        GetPrimaryWindow()->OnFileDrop += [&](std::string file)
        {
            m_Layout.LoadProject(file);
        };

        m_Layout.LoadLayout("layout.ini");
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

    virtual void OnResize(Nexus::Point2D<uint32_t> size) override
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