#pragma once
#include "Core/Graphics/GraphicsDevice.h"
#include "glm/glm.hpp"

namespace Nexus
{
    class Renderer
    {
    public:
        void Begin(const glm::mat4 &vp, const glm::vec4 &clearColor)
        {
            this->m_VP = vp;
        }

        void End()
        {
        }

    public:
        static Renderer *Create(std::shared_ptr<GraphicsDevice> device)
        {
            return new Renderer(device);
        }

    private:
        Renderer(std::shared_ptr<GraphicsDevice> device)
        {
            this->m_GraphicsDevice = device;
        }

    private:
        std::shared_ptr<GraphicsDevice> m_GraphicsDevice;
        glm::mat4 m_VP;
    };
}