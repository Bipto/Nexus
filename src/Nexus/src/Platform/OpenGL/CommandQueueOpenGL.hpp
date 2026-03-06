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
		const CommandQueueDescription &GetDescription() const final;
		Ref<ISwapchain>				   CreateSwapchain(const SwapchainDescription &spec) final;
		void						   SubmitCommandList(Ref<ICommandList> commandList) final;
		void						   SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence) final;
		void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists) final;
		virtual void				   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence) final;
		IGraphicsDevice				  *GetGraphicsDevice() final;
		bool						   WaitForIdle() final;
		Ref<ICommandList>			   CreateCommandList(const CommandListDescription &spec = {}) final;

	  private:
		GraphicsDeviceOpenGL   *m_Device		  = nullptr;
		CommandQueueDescription m_Description	  = {};
		CommandExecutorOpenGL	m_CommandExecutor = {};
	};
}	 // namespace Nexus::Graphics