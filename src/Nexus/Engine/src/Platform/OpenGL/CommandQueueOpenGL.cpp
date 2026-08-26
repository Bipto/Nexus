#include "CommandQueueOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "Profiling/Profiler.hpp"
#include "SwapchainOpenGL.hpp"

namespace Nexus::Graphics
{
    CommandQueueOpenGL::CommandQueueOpenGL(GraphicsDeviceOpenGL *device, const CommandQueueDescription &description)
        : m_Device(device), m_Description(description)
    {
    }

    CommandQueueOpenGL::~CommandQueueOpenGL()
    {
    }

    const CommandQueueDescription &CommandQueueOpenGL::GetDescription() const
    {
        return m_Description;
    }

    SwapchainHandle CommandQueueOpenGL::CreateSwapchain(const SwapchainDescription &spec)
    {
        auto swapchain = std::make_unique<SwapchainOpenGL>(spec, m_Device);
        return m_Resources.Swapchains.CreateShared(std::move(swapchain));
    }

    void CommandQueueOpenGL::SubmitCommandList(CommandListHandle commandList, std::optional<FenceHandle> fence)
    {
        SubmitCommandLists(&commandList, 1, fence);
    }

    void CommandQueueOpenGL::SubmitCommandLists(CommandListHandle *commandLists, uint32_t numCommandLists,
                                                std::optional<FenceHandle> fence)
    {
        NX_PROFILE_FUNCTION();

        for (uint32_t i = 0; i < numCommandLists; i++)
        {
            CommandListOpenGL *commandList = commandLists[i].AsDerived<CommandListOpenGL>();
            m_CommandExecutor.ExecuteCommands(commandList, m_Device);
            m_CommandExecutor.Reset();
        }
    }

    IGraphicsDevice *CommandQueueOpenGL::GetGraphicsDevice()
    {
        return m_Device;
    }

    bool CommandQueueOpenGL::WaitForIdle()
    {
        NX_PROFILE_FUNCTION();
        return true;
    }

    CommandListHandle CommandQueueOpenGL::CreateCommandList(const CommandListDescription &spec)
    {
        auto commandList = std::make_unique<CommandListOpenGL>(spec);
        return m_Resources.CommandLists.CreateShared(std::move(commandList));
    }
} // namespace Nexus::Graphics