#pragma once

#include "CommandExecutorOpenGL.hpp"

#include "GraphicsDeviceOpenGL.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/CommandQueue.hpp"
#include "RHI/Fence.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/Swapchain.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
    class CommandQueueOpenGL final : public ICommandQueue
    {
      public:
        CommandQueueOpenGL(GraphicsDeviceOpenGL *device, const CommandQueueDescription &description);
        virtual ~CommandQueueOpenGL();
        const CommandQueueDescription &GetDescription() const final;
        SwapchainHandle CreateSwapchain(const SwapchainDescription &spec) final;
        void SubmitCommandList(CommandListHandle commandList, std::optional<FenceHandle> fence = {}) final;
        void SubmitCommandLists(
            CommandListHandle *commandLists, uint32_t numCommandLists, std::optional<FenceHandle> fence = {}
        ) final;
        IGraphicsDevice *GetGraphicsDevice() final;
        bool WaitForIdle() final;
        CommandListHandle CreateCommandList(const CommandListDescription &spec = {}) final;

      private:
        GraphicsDeviceOpenGL *m_Device = nullptr;
        CommandQueueDescription m_Description = {};
        CommandExecutorOpenGL m_CommandExecutor = {};

        CommandQueueResourceManager m_Resources = {};
    };
} // namespace Nexus::Graphics