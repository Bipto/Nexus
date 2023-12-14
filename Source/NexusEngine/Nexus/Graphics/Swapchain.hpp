#pragma once

#include <vector>
#include <functional>

#include "Nexus/ApplicationSpecification.hpp"

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
        virtual ~Swapchain()
        {
            for (const auto &function : m_ClosingFunctions)
            {
                function();
            }
        }

        void OnClose(std::function<void(void)> function)
        {
            m_ClosingFunctions.push_back(function);
        }

        virtual void Initialise() = 0;
        virtual void SwapBuffers() = 0;
        virtual VSyncState GetVsyncState() = 0;
        virtual void SetVSyncState(VSyncState vsyncState) = 0;
        virtual Window *GetWindow() = 0;
        virtual void Prepare() = 0;

    private:
        std::vector<std::function<void(void)>> m_ClosingFunctions;

        friend class GraphicsDevice;
        friend class RenderPass;
    };
}