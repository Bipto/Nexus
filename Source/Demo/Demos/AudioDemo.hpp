#pragma once

#include "Demo.hpp"

namespace Demos
{
    class AudioDemo : public Demo
    {
    public:
        AudioDemo(const std::string &name, Nexus::Application *app)
            : Demo(name, app)
        {
            Nexus::Graphics::RenderPassSpecification spec;
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, app->GetPrimaryWindow()->GetSwapchain());

            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_AudioBuffer = m_AudioDevice->CreateAudioBufferFromWavFile({Nexus::FileSystem::GetFilePathAbsolute("resources/audio/laser_shoot.wav")});
            m_AudioSource = m_AudioDevice->CreateAudioSource(m_AudioBuffer);
        }

        virtual ~AudioDemo()
        {
            delete m_CommandList;
            delete m_RenderPass;
            delete m_AudioBuffer;
            delete m_AudioSource;
        }

        virtual void Update(Nexus::Time time) override
        {
        }

        virtual void Render(Nexus::Time time) override
        {
            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                m_ClearColour.r,
                m_ClearColour.g,
                m_ClearColour.b,
                1.0f};

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
            m_CommandList->EndRenderPass();
            m_CommandList->End();

            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::Text("Press button to play a sound effect");
            if (ImGui::Button("Play"))
            {
                m_AudioDevice->PlaySource(m_AudioSource);
            }
        }

    private:
        Nexus::Graphics::CommandList *m_CommandList;
        Nexus::Graphics::RenderPass *m_RenderPass;
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

        Nexus::Audio::AudioBuffer *m_AudioBuffer;
        Nexus::Audio::AudioSource *m_AudioSource;
    };
}