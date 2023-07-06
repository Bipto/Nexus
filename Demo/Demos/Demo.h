#pragma once

#include "Core/Memory.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Time.h"

namespace Demos
{
    class Demo
    {
    public:
        Demo(const std::string &name, Nexus::Application *app)
            : m_Name(name),
              m_GraphicsDevice(app->GetGraphicsDevice()),
              m_Window(app->GetWindow())
        {
        }

        virtual void Update(Nexus::Time time)
        {
        }
        virtual void Render(Nexus::Time time) {}

        virtual void OnResize(Nexus::Point<int> size) {}
        virtual void RenderUI() {}

        const std::string &GetName() { return m_Name; }

    protected:
        std::string m_Name;
        Nexus::Ref<Nexus::Graphics::GraphicsDevice> m_GraphicsDevice = nullptr;
        Nexus::Window *m_Window = nullptr;
    };
}