#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "GraphicsDeviceVk.hpp"
#include "PipelineVk.hpp"

#include "CommandExecutorVk.hpp"

namespace Nexus::Graphics
{
    class CommandListVk : public CommandList
    {
    public:
        CommandListVk(GraphicsDeviceVk *graphicsDevice, const CommandListSpecification& spec);
        virtual ~CommandListVk();

        VkCommandBuffer &GetCurrentCommandBuffer();
        VkSemaphore &GetCurrentSemaphore();
        VkFence &GetCurrentFence();

    private:
        GraphicsDeviceVk *m_Device;
        CommandRecorder m_CommandRecorder{};

        VkCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
        VkCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
        VkSemaphore m_RenderSemaphores[FRAMES_IN_FLIGHT];
        VkFence m_RenderFences[FRAMES_IN_FLIGHT];
    };
}

#endif