#include "NexusEngine.hpp"

class TestApplication : public Nexus::Application
{
public:
    TestApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
    }

    virtual void Load() override
    {
        m_CommandList = m_GraphicsDevice->CreateCommandList();

        Nexus::Graphics::RenderPassSpecification spec;
        spec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.StencilDepthLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.RenderToSwapchain = true;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->BeginFrame();
        Nexus::Graphics::RenderPassBeginInfo beginInfo;
        beginInfo.ClearColorValue = {
            1.0f,
            0.0f,
            0.0f,
            1.0f};

        m_CommandList->Begin();

        m_CommandList->End();
        // m_GraphicsDevice->SubmitCommandList(m_CommandList);

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        m_GraphicsDevice->Resize(size);
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
    Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::Vulkan;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = false;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new TestApplication(spec);
}