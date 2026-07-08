#include "FenceOpenGL.hpp"

#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
    FenceOpenGL::FenceOpenGL(const FenceDescription &desc, GraphicsDeviceOpenGL *device)
        : m_Description(desc), m_Device(device)
    {
        CreateFence();
    }

    FenceOpenGL::~FenceOpenGL()
    {
        DestroyFence();
    }

    bool FenceOpenGL::IsSignalled() const
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        return context->IsSignalled(m_Sync);
    }

    const FenceDescription &FenceOpenGL::GetDescription() const
    {
        return m_Description;
    }

    GLsync FenceOpenGL::GetHandle() const
    {
        return m_Sync;
    }

    void FenceOpenGL::Reset()
    {
        DestroyFence();
        CreateFence();
    }

    GLenum FenceOpenGL::Wait(uint64_t timeoutNS)
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        return context->WaitForFence(m_Sync, timeoutNS);
    }

    void FenceOpenGL::CreateFence()
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        m_Sync = context->CreateFence(m_Description).value();
    }

    void FenceOpenGL::DestroyFence()
    {
        GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
        context->DestroyFence(m_Sync);
    }
} // namespace Nexus::Graphics
