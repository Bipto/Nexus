#pragma once

#include "Nexus-Core/ApplicationSpecification.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
    // forward declaration
    class Window;
}

namespace Nexus::Graphics
{
    class Swapchain
    {
    public:
        Swapchain(const SwapchainSpecification &spec)
            : m_Specification(spec)
        {
        }
        virtual ~Swapchain() {}

        virtual void Initialise() = 0;
        virtual void SwapBuffers() = 0;
        virtual VSyncState GetVsyncState() = 0;
        virtual void SetVSyncState(VSyncState vsyncState) = 0;
        virtual Window *GetWindow() = 0;
        virtual void Prepare() = 0;

        const SwapchainSpecification &GetSpecification()
        {
            return m_Specification;
        }

    protected:
        SwapchainSpecification m_Specification;

    private:
        friend class GraphicsDevice;
    };
}