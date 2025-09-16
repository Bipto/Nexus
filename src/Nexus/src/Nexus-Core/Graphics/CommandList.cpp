#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/CommandExecutor.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
	void CommandBuffer::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		for (const auto &header : m_Headers)
		{
			IGraphicsCommand *command = reinterpret_cast<IGraphicsCommand *>(m_CommandData.data() + header.Offset);
			command->Execute(executor, device);
		}
	}

	CommandList::CommandList(const CommandListDescription &spec) : m_Description(spec)
	{
	}

	void CommandList::Begin()
	{
		NX_PROFILE_FUNCTION();

		if (m_Started)
		{
			NX_ERROR("Attempting to begin a command into a CommandList that has not "
					 "been closed");
		}

		m_Started	  = true;
		m_DebugGroups = 0;
		m_CommandBuffer.Reset();
	}

	void CommandList::End()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to end a CommandList but the CommandList was not begun");
		}

		// clean up any unclosed debug groups
		for (uint32_t i = 0; i < m_DebugGroups; i++) { EndDebugGroup(); }

		// reset the debug group counter
		m_DebugGroups = 0;

		// end recording into the CommandList
		m_Started = false;
	}

	void CommandList::SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetVertexBufferCommand command;
		command.View = vertexBuffer;
		command.Slot = slot;

		SetVertexBufferCommandImpl commandImpl = {};
		commandImpl.Command					   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetIndexBuffer(IndexBufferView indexBuffer)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetIndexBufferCommand command;
		command.View = indexBuffer;

		SetIndexBufferCommandImpl commandImpl = {};
		commandImpl.Command					  = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetPipeline(Ref<Pipeline> pipeline)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetPipelineCommand command = {};
		command.PipelineToBind	   = pipeline;

		SetPipelineCommandImpl commandImpl = {};
		commandImpl.Command				   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::Draw(const DrawDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawCommandImpl commandImpl = {};
		commandImpl.Command			= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DrawIndexed(const DrawIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawIndexedCommandImpl commandImpl = {};
		commandImpl.Command				   = desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DrawIndirect(const DrawIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawIndirectCommandImpl commandImpl = {};
		commandImpl.Command					= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawIndirectIndexedCommandImpl commandImpl = {};
		commandImpl.Command						   = desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::Dispatch(const DispatchDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DispatchCommandImpl commandImpl = {};
		commandImpl.Command				= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DispatchIndirect(const DispatchIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DispatchIndirectCommandImpl commandImpl = {};
		commandImpl.Command						= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DrawMesh(const DrawMeshDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawMeshCommandImpl commandImpl = {};
		commandImpl.Command				= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawMeshIndirectCommandImpl commandImpl = {};
		commandImpl.Command						= desc;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetResourceSet(Ref<ResourceSet> resources)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetResourceSetCommandImpl commandImpl = {};
		commandImpl.Command					  = resources;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::ClearColourTarget(uint32_t index, const ClearColorValue &color, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColourTargetCommand command;
		command.Index = index;
		command.Color = color;
		command.Rect  = clearRect;

		ClearColourTargetCommandImpl commandImpl = {};
		commandImpl.Command						 = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::ClearColourTarget(uint32_t index, const ClearColorValue &color)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColourTargetCommand command;
		command.Index = index;
		command.Color = color;
		command.Rect  = std::nullopt;

		ClearColourTargetCommandImpl commandImpl = {};
		commandImpl.Command						 = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearDepthStencilTargetCommand command;
		command.Value = value;
		command.Rect  = clearRect;

		ClearDepthStencilTargetCommandImpl commandImpl = {};
		commandImpl.Command							   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearDepthStencilTargetCommand command;
		command.Value = value;
		command.Rect  = std::nullopt;

		ClearDepthStencilTargetCommandImpl commandImpl = {};
		commandImpl.Command							   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetRenderTarget(RenderTarget target)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetRenderTargetCommandImpl commandImpl = {};
		commandImpl.Command					   = target;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetViewport(const Viewport &viewport)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetViewportCommandImpl commandImpl = {};
		commandImpl.Command				   = viewport;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::SetScissor(const Scissor &scissor)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetScissorCommandImpl commandImpl = {};
		commandImpl.Command				  = scissor;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ResolveSamplesToSwapchainCommand command;
		command.Source		= source;
		command.SourceIndex = sourceIndex;
		command.Target		= target;

		ResolveCommandImpl commandImpl = {};
		commandImpl.Command			   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void Nexus::Graphics::CommandList::StartTimingQuery(Ref<TimingQuery> query)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		StartTimingQueryCommand command;
		command.Query = query;

		StartTimingQueryCommandImpl commandImpl = {};
		commandImpl.Command						= command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void Nexus::Graphics::CommandList::StopTimingQuery(Ref<TimingQuery> query)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		StopTimingQueryCommand command;
		command.Query = query;

		StopTimingQueryCommandImpl commandImpl = {};
		commandImpl.Command					   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyBufferToBufferCommand command;
		command.BufferCopy = bufferCopy;

		CopyBufferToBufferCommandImpl commandImpl = {};
		commandImpl.Command						  = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyBufferToTextureCommand command;
		command.BufferTextureCopy = bufferTextureCopy;

		CopyBufferToTextureCommandImpl commandImpl = {};
		commandImpl.Command						   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyTextureToBufferCommand command;
		command.TextureBufferCopy = textureBufferCopy;

		CopyTextureToBufferCommandImpl commandImpl = {};
		commandImpl.Command						   = command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyTextureToTextureCommand command;
		command.TextureCopy = textureCopy;

		CopyTextureToTextureCommandImpl commandImpl = {};
		commandImpl.Command							= command;
		m_CommandBuffer.AddCommand(commandImpl);
	}

	void CommandList::BeginDebugGroup(const std::string &name)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		BeginDebugGroupCommand command;
		command.GroupName = name;

		m_DebugGroups++;
	}

	void CommandList::EndDebugGroup()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		EndDebugGroupCommand command;

		m_DebugGroups--;
	}

	void CommandList::InsertDebugMarker(const std::string &name)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		InsertDebugMarkerCommand command;
		command.MarkerName = name;
	}

	void CommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		NX_PROFILE_FUNCTION();

		SetBlendFactorCommand command;
		command.BlendFactorDesc = blendFactor;
	}

	void CommandList::SetStencilReference(uint32_t stencilReference)
	{
		NX_PROFILE_FUNCTION();

		SetStencilReferenceCommand command;
		command.StencilReference = stencilReference;
	}

	void CommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
		NX_PROFILE_FUNCTION();

		BuildAccelerationStructuresCommand command;
		command.BuildDescriptions = description;
	}

	void CommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
	}

	void CommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
	}

	void CommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();
	}

	void CommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
		NX_PROFILE_FUNCTION();

		PushConstantsDesc pushConstantDesc = {};
		pushConstantDesc.Name			   = name;
		pushConstantDesc.Offset			   = offset;
		pushConstantDesc.Data.resize(size);
		memcpy(pushConstantDesc.Data.data(), data, size);
	}

	const CommandListDescription &CommandList::GetDescription()
	{
		NX_PROFILE_FUNCTION();

		return m_Description;
	}

	CommandBuffer &CommandList::GetCommandBuffer()
	{
		return m_CommandBuffer;
	}

	const CommandBuffer &CommandList::GetCommandBuffer() const
	{
		return m_CommandBuffer;
	}

	bool CommandList::IsRecording() const
	{
		NX_PROFILE_FUNCTION();

		return m_Started;
	}

	void SetVertexBufferCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetIndexBufferCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetPipelineCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}
	void DrawCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DrawIndexedCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DrawIndirectCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DrawIndirectIndexedCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DispatchCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DispatchIndirectCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DrawMeshCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DrawMeshIndirectCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetResourceSetCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void ClearColourTargetCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void ClearDepthStencilTargetCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetRenderTargetCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetViewportCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetScissorCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void ResolveCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void StartTimingQueryCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void StopTimingQueryCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void CopyBufferToBufferCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void CopyBufferToTextureCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void CopyTextureToBufferCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void CopyTextureToTextureCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void BeginDebugGroupCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void EndDebugGroupCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void InsertDebugMarkerCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetBlendFactorCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void SetStencilRefCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void BuildAccelerationStructuresCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void AccelerationStructureCopyCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void AccelerationStructureDeviceBufferCopyCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void DeviceBufferAccelerationStructureCopyCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}

	void PushConstantsCommandImpl::Execute(CommandExecutor *executor, GraphicsDevice *device)
	{
		executor->ExecuteCommand(Command, device);
	}
}	 // namespace Nexus::Graphics
