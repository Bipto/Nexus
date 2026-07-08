#pragma once

#include "CommandExecutorVk.hpp"
#include "GraphicsDeviceVk.hpp"

#include "RHI/CommandQueue.hpp"
#include <RHI/CommandList.hpp>
#include <RHI/Fence.hpp>
#include <RHI/GraphicsDevice.hpp>
#include <RHI/Swapchain.hpp>
#include <RHI/Types.hpp>

namespace Nexus::Graphics
{
    class CommandQueueVk final : public ICommandQueue
    {
      public:
        CommandQueueVk(GraphicsDeviceVk *device, const CommandQueueDescription &description);
        virtual ~CommandQueueVk();
        const CommandQueueDescription &GetDescription() const final;
        SwapchainHandle CreateSwapchain(const SwapchainDescription &spec) final;
        void SubmitCommandList(CommandListHandle commandList) final;
        void SubmitCommandList(CommandListHandle commandList, Ref<IFence> fence) final;
        void SubmitCommandLists(CommandListHandle *commandLists, uint32_t numCommandLists) final;
        void SubmitCommandLists(CommandListHandle *commandLists, uint32_t numCommandLists, Ref<IFence> fence) final;
        IGraphicsDevice *GetGraphicsDevice() final;
        bool WaitForIdle() final;
        VkQueue GetVkQueue() const;
        CommandListHandle CreateCommandList(const CommandListDescription &spec = {}) final;

      private:
        GraphicsDeviceVk *m_Device = nullptr;
        CommandQueueDescription m_Description = {};
        VkQueue m_Queue = VK_NULL_HANDLE;
        std::unique_ptr<CommandExecutorVk> m_CommandExecutor = nullptr;

        CommandQueueResourceManager m_Resources = {};
    };
} // namespace Nexus::Graphics