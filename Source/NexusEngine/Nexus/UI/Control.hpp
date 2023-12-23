#pragma once

#include "Nexus/Input/InputState.hpp"
#include "Nexus/Renderer/BatchRenderer.hpp"

namespace Nexus::UI
{
    class Control
    {
    public:
        void Initialise(Nexus::InputState *inputState);
        virtual void Render(Nexus::Graphics::BatchRenderer *batchRenderer, Nexus::Graphics::RenderTarget target) = 0;
        virtual void Update() = 0;

    protected:
        Nexus::InputState *m_InputState = nullptr;
    };
}