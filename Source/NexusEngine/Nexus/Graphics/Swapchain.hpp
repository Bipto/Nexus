#pragma once

namespace Nexus::Graphics
{
    /// @brief An enum class representing whether VSync is enabled
    enum class VSyncState
    {
        /// @brief The graphics card will render as many frames as possible or will be limited by the frame rate cap specified
        Disabled = 0,

        /// @brief The refresh rate will be synchronised to the monitor
        Enabled = 1
    };

    class Swapchain
    {
    public:
        virtual ~Swapchain() {}
        virtual void SwapBuffers() = 0;
        virtual VSyncState GetVsyncState() = 0;
        virtual void SetVSyncState(VSyncState vsyncState) = 0;
    };
}