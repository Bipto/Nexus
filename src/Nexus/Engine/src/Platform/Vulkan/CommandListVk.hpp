#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "CommandQueueVk.hpp"
#include "GraphicsDeviceVk.hpp"
#include "PipelineVk.hpp"
#include "RHI/CommandList.hpp"

namespace Nexus::Graphics
{
    class CommandListVk : public ICommandList
    {
      public:
        CommandListVk(
            GraphicsDeviceVk *graphicsDevice, CommandQueueVk *commandQueue, const CommandListDescription &spec
        );
        virtual ~CommandListVk();

        VkCommandBuffer &GetCurrentCommandBuffer();

      private:
        GraphicsDeviceVk *m_Device = nullptr;
        CommandQueueVk *m_Queue = nullptr;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VkCommandBuffer m_CommandBuffer;
    };
} // namespace Nexus::Graphics

#endif