#include "CommandExecutorSoftware.hpp"

namespace Nexus::Graphics
{
	CommandExecutorSoftware::CommandExecutorSoftware()
	{
	}

	CommandExecutorSoftware::~CommandExecutorSoftware()
	{
		Reset();
	}

	void CommandExecutorSoftware::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
	{
		for (const auto &element : commands)
		{
			std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
		}
	}

	void CommandExecutorSoftware::Reset()
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(DrawElementCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device)
	{
	}

	void CommandExecutorSoftware::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
	}
}	 // namespace Nexus::Graphics
