#pragma once

#include "NexusEngine.hpp"
#include "Nexus/FileSystem/FileSystem.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Demos
{
    class Demo
    {
    public:
        Demo(const std::string &name, Nexus::Application *app)
            : m_Name(name),
              m_GraphicsDevice(app->GetGraphicsDevice()),
              m_AudioDevice(app->GetAudioDevice()),
              m_Window(app->GetPrimaryWindow())
        {
        }

        virtual ~Demo() {}

        virtual void Update(Nexus::Time time)
        {
        }
        virtual void Render(Nexus::Time time) {}

        virtual void OnResize(Nexus::Point<uint32_t> size) {}
        virtual void RenderUI() {}

        const std::string &GetName() { return m_Name; }

    protected:
        std::string m_Name;
        Nexus::Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;
        Nexus::Audio::AudioDevice *m_AudioDevice = nullptr;
        Nexus::Window *m_Window = nullptr;
    };
}