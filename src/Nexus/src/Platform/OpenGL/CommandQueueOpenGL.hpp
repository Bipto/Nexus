#pragma once

#include "CommandExecutorOpenGL.hpp"
#include "Nexus-Core/Graphics/CommandQueue.hpp"

#include "GraphicsDeviceOpenGL.hpp"
#include <Nexus-Core/Graphics/CommandList.hpp>
#include <Nexus-Core/Graphics/Fence.hpp>
#include <Nexus-Core/Graphics/GraphicsDevice.hpp>
#include <Nexus-Core/Graphics/Swapchain.hpp>
#include <Nexus-Core/Types.hpp>

namespace Nexus::Graphics
{
	class CommandQueueOpenGL final : public ICommandQueue
	{
	  public:
		CommandQueueOpenGL(GraphicsDeviceOpenGL *device, const CommandQueueDescription &description);
		virtual ~CommandQueueOpenGL();
		void			SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) final;
		void			Present(Ref<Swapchain> swapchain) final;
		GraphicsDevice *GetGraphicsDevice() final;
		bool			WaitForIdle() final;

	  private:
		GraphicsDeviceOpenGL   *m_Device		  = nullptr;
		CommandQueueDescription m_Description	  = {};
		CommandExecutorOpenGL	m_CommandExecutor = {};
	};
}	 // namespace Nexus::Graphics