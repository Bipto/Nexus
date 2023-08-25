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
            m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());

            m_CommandList = m_GraphicsDevice->CreateCommandList();

            m_AudioBuffer = m_AudioDevice->CreateAudioBufferFromWavFile("Resources/Audio/Laser_Shoot.wav");
            m_AudioSource = m_AudioDevice->CreateAudioSource(m_AudioBuffer);
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

            if (Nexus::Input::IsKeyPressed(Nexus::KeyCode::Space))
            {
                m_AudioDevice->PlaySource(m_AudioSource);
            }
        }

        virtual void OnResize(Nexus::Point<int> size) override
        {
        }

        virtual void RenderUI() override
        {
            ImGui::Text("Press the spacebar to play a sound effect");
        }

    private:
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
        Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
        glm::vec3 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

        Nexus::Ref<Nexus::Audio::AudioBuffer> m_AudioBuffer;
        Nexus::Ref<Nexus::Audio::AudioSource> m_AudioSource;
    };
}