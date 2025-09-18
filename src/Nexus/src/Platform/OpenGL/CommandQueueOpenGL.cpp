#include "CommandQueueOpenGL.hpp"
#include "CommandListOpenGL.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

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

	void CommandQueueOpenGL::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
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

	void CommandQueueOpenGL::Present(Ref<Swapchain> swapchain)
	{
		NX_PROFILE_FUNCTION();

		Ref<SwapchainOpenGL> swapchainGL = std::dynamic_pointer_cast<SwapchainOpenGL>(swapchain);
		swapchainGL->SwapBuffers();
	}

	GraphicsDevice *CommandQueueOpenGL::GetGraphicsDevice()
	{
		return m_Device;
	}

	bool CommandQueueOpenGL::WaitForIdle()
	{
		NX_PROFILE_FUNCTION();
		return true;
	}
}	 // namespace Nexus::Graphics