#include "CommandQueueVk.hpp"

#include "Profiling/Profiler.hpp"

#include "CommandListVk.hpp"
#include "FenceVk.hpp"
#include "SwapchainVk.hpp"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    CommandQueueVk::CommandQueueVk(GraphicsDeviceVk *device, const CommandQueueDescription &description)
        : m_Device(device), m_Description(description)
    {
        m_Queue = Vk::GetDeviceQueue(device, description);
        device->SetObjectName(VK_OBJECT_TYPE_QUEUE, (uint64_t)m_Queue, description.DebugName.c_str());

        m_CommandExecutor = std::make_unique<CommandExecutorVk>(device);
    }

    CommandQueueVk::~CommandQueueVk()
    {
    }

    const CommandQueueDescription &CommandQueueVk::GetDescription() const
    {
        return m_Description;
    }

    SwapchainHandle CommandQueueVk::CreateSwapchain(const SwapchainDescription &spec)
    {
        auto swapchain = std::make_unique<SwapchainVk>(m_Device, this, spec);
        std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk =
            std::dynamic_pointer_cast<PhysicalDeviceVk>(m_Device->GetPhysicalDevice());

        const GladVulkanContext &context = m_Device->GetVulkanContext();

        VkBool32 presentSupport = false;
        context.GetPhysicalDeviceSurfaceSupportKHR(
            physicalDeviceVk->GetVkPhysicalDevice(), m_Description.QueueFamilyIndex, swapchain->GetSurface(),
            &presentSupport
        );

        if (!presentSupport)
        {
            throw std::runtime_error("Device is unable to present to this swapchain");
        }

        return m_Resources.Swapchains.CreateShared(std::move(swapchain));
    }

    void CommandQueueVk::SubmitCommandList(CommandListHandle commandList, std::optional<FenceHandle> fence)
    {
        SubmitCommandLists(&commandList, 1, fence);
    }

    void CommandQueueVk::SubmitCommandLists(
        CommandListHandle *commandLists, uint32_t numCommandLists, std::optional<FenceHandle> fence
    )
    {
        SubmitCommandLists(commandLists, numCommandLists, nullptr, 0, nullptr, 0, fence);
    }

    void CommandQueueVk::SubmitCommandLists(
        CommandListHandle *commandLists, uint32_t numCommandLists, const VkSemaphore *waitSemaphores,
        uint32_t waitSemaphoreCount, const VkSemaphore *signalSemaphores, uint32_t signalSemaphoreCount,
        std::optional<FenceHandle> fence
    )
    {
        NX_PROFILE_FUNCTION();

        VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        VkPipelineStageFlags2 waitDestStageMask2 = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

        std::vector<VkCommandBuffer> commandBuffers(numCommandLists);

        VkFence nativeFence = VK_NULL_HANDLE;

        if (fence)
        {
            FenceVk *vulkanFence = fence->AsDerived<FenceVk>();
            nativeFence = vulkanFence->GetHandle();
        }

        // record the commands into the actual vulkan command list
        for (uint32_t i = 0; i < numCommandLists; i++)
        {
            CommandListVk *commandList = commandLists[i].AsDerived<CommandListVk>();
            const std::vector<std::unique_ptr<IGraphicsCommand>> &commands = commandList->GetCommands();
            m_CommandExecutor->ExecuteCommands(commandList, m_Device);
            m_CommandExecutor->Reset();
            commandBuffers[i] = commandList->GetCurrentCommandBuffer();
        }

        NX_VALIDATE(
            Vk::SubmitQueue(
                m_Device, m_Queue, commandBuffers, waitDestStageMask, waitDestStageMask2, nativeFence, waitSemaphores,
                waitSemaphoreCount, signalSemaphores, signalSemaphoreCount
            ) == VK_SUCCESS,
            "Failed to submit queue"
        );
    }

    IGraphicsDevice *CommandQueueVk::GetGraphicsDevice()
    {
        return m_Device;
    }

    bool CommandQueueVk::WaitForIdle()
    {
        const GladVulkanContext &context = m_Device->GetVulkanContext();

        VkResult result = context.QueueWaitIdle(m_Queue);
        if (result == VK_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    VkQueue CommandQueueVk::GetVkQueue() const
    {
        return m_Queue;
    }

    CommandListHandle CommandQueueVk::CreateCommandList(const CommandListDescription &spec)
    {
        auto commandList = std::make_unique<CommandListVk>(m_Device, this, spec);
        return m_Resources.CommandLists.CreateShared(std::move(commandList));
    }
} // namespace Nexus::Graphics
