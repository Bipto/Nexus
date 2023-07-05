#include "Renderer.h"

namespace Nexus
{
    void Renderer::Begin(const glm::mat4 &vp, const glm::vec4 &clearColor)
    {
        this->m_VP = vp;
    }

    void Renderer::End()
    {
    }

    Renderer *Renderer::Create(Ref<Graphics::GraphicsDevice> device)
    {
        return new Renderer(device);
    }

    Renderer::Renderer(Ref<Graphics::GraphicsDevice> device)
    {
        this->m_GraphicsDevice = device;
    }
}