#include "CommandQueueOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "Platform/Timings/Profiler.hpp"
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

	Ref<ISwapchain> CommandQueueOpenGL::CreateSwapchain(IWindow *window, const SwapchainDescription &spec)
	{
		Ref<PhysicalDeviceOpenGL> physicalDevice = m_Device->GetPhysicalDeviceOpenGL();
		GL::SetCurrentContext(physicalDevice->GetOffscreenContext());
		return CreateRef<SwapchainOpenGL>(window, spec, m_Device);
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