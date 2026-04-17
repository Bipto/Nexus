#include "CommandQueueOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "Profiling/Profiler.hpp"
#include "SwapchainOpenGL.hpp"

namespace Nexus::Graphics
{
	CommandQueueOpenGL::CommandQueueOpenGL(GraphicsDeviceOpenGL *device, const CommandQueueDescription &description)
		: m_Device(device),
		  m_Description(description)
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
		Ref<PhysicalDeviceOpenGL> physicalDevice = m_Device->GetPhysicalDeviceOpenGL();

		GL::SetCurrentContext(physicalDevice->GetOffscreenContext());
		auto swapchain = std::make_unique<SwapchainOpenGL>(spec, m_Device);
		return m_Resources.Swapchains.CreateShared(std::move(swapchain));
	}

	void CommandQueueOpenGL::SubmitCommandList(Ref<ICommandList> commandList)
	{
		SubmitCommandList(commandList, nullptr);
	}

	void CommandQueueOpenGL::SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence)
	{
		SubmitCommandLists(&commandList, 1, fence);
	}

	void CommandQueueOpenGL::SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists)
	{
		SubmitCommandLists(commandLists, numCommandLists, nullptr);
	}

	void CommandQueueOpenGL::SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence)
	{
		NX_PROFILE_FUNCTION();

		// Ref<PhysicalDeviceOpenGL> physicalDevice = m_Device->GetPhysicalDeviceOpenGL();
		// GL::SetCurrentContext(physicalDevice->GetOffscreenContext());

		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListOpenGL> commandList = std::dynamic_pointer_cast<CommandListOpenGL>(commandLists[i]);
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

	Ref<ICommandList> CommandQueueOpenGL::CreateCommandList(const CommandListDescription &spec)
	{
		return CreateRef<CommandListOpenGL>(spec);
	}
}	 // namespace Nexus::Graphics