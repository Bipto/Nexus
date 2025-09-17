#include "CommandQueueOpenGL.hpp"
#include "CommandListOpenGL.hpp"

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
		Ref<PhysicalDeviceOpenGL> physicalDevice = m_Device->GetPhysicalDeviceOpenGL();
		GL::SetCurrentContext(physicalDevice->GetOffscreenContext());

		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListOpenGL>								   commandList = std::dynamic_pointer_cast<CommandListOpenGL>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			m_CommandExecutor.ExecuteCommands(commands, m_Device);
			m_CommandExecutor.Reset();
		}
	}

	void CommandQueueOpenGL::Present(Ref<Swapchain> swapchain)
	{
		Ref<SwapchainOpenGL> swapchainGL = std::dynamic_pointer_cast<SwapchainOpenGL>(swapchain);
		swapchainGL->SwapBuffers();
	}

	GraphicsDevice *CommandQueueOpenGL::GetGraphicsDevice()
	{
		return m_Device;
	}

	bool CommandQueueOpenGL::WaitForIdle()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.Finish(); });
		return true;
	}
}	 // namespace Nexus::Graphics