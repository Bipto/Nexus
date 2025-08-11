#include "Nexus-Core/Graphics/CommandList.hpp"

namespace Nexus::Graphics
{
	CommandList::CommandList(const CommandListDescription &spec) : m_Description(spec)
	{
	}

	void CommandList::Begin()
	{
		if (m_Started)
		{
			NX_ERROR("Attempting to begin a command into a CommandList that has not "
					 "been closed");
		}

		m_Commands.clear();
		m_Started	  = true;
		m_DebugGroups = 0;
	}

	void CommandList::End()
	{
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
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetVertexBufferCommand command;
		command.View = vertexBuffer;
		command.Slot = slot;
		m_Commands.push_back(command);
	}

	void CommandList::SetIndexBuffer(IndexBufferView indexBuffer)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetIndexBufferCommand command;
		command.View = indexBuffer;
		m_Commands.push_back(command);
	}

	void CommandList::SetPipeline(Ref<Pipeline> pipeline)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(pipeline);
	}

	void CommandList::Draw(const DrawDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DrawIndexed(const DrawIndexedDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DrawIndirect(const DrawIndirectDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::Dispatch(const DispatchDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DispatchIndirect(const DispatchIndirectDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DrawMesh(const DrawMeshDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
	}

	void CommandList::SetResourceSet(Ref<ResourceSet> resources)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(resources);
	}

	void CommandList::ClearColorTarget(uint32_t index, const ClearColorValue &color, ClearRect clearRect)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColorTargetCommand command;
		command.Index = index;
		command.Color = color;
		command.Rect  = clearRect;
		m_Commands.push_back(command);
	}

	void CommandList::ClearColorTarget(uint32_t index, const ClearColorValue &color)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColorTargetCommand command;
		command.Index = index;
		command.Color = color;
		command.Rect  = std::nullopt;
		m_Commands.push_back(command);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearDepthStencilTargetCommand command;
		command.Value = value;
		command.Rect  = clearRect;
		m_Commands.push_back(command);
	}

	void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearDepthStencilTargetCommand command;
		command.Value = value;
		command.Rect  = std::nullopt;
		m_Commands.push_back(command);
	}

	void CommandList::SetRenderTarget(RenderTarget target)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(target);
	}

	void CommandList::SetViewport(const Viewport &viewport)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(viewport);
	}

	void CommandList::SetScissor(const Scissor &scissor)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(scissor);
	}

	void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Ref<Swapchain> target)
	{
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
		m_Commands.push_back(command);
	}

	void Nexus::Graphics::CommandList::StartTimingQuery(Ref<TimingQuery> query)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		StartTimingQueryCommand command;
		command.Query = query;
		m_Commands.push_back(command);
	}

	void Nexus::Graphics::CommandList::StopTimingQuery(Ref<TimingQuery> query)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		StopTimingQueryCommand command;
		command.Query = query;
		m_Commands.push_back(command);
	}

	void CommandList::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyBufferToBufferCommand command;
		command.BufferCopy = bufferCopy;
		m_Commands.push_back(command);
	}

	void CommandList::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyBufferToTextureCommand command;
		command.BufferTextureCopy = bufferTextureCopy;
		m_Commands.push_back(command);
	}

	void CommandList::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyTextureToBufferCommand command;
		command.TextureBufferCopy = textureBufferCopy;
		m_Commands.push_back(command);
	}

	void CommandList::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		Graphics::CopyTextureToTextureCommand command;
		command.TextureCopy = textureCopy;
		m_Commands.push_back(command);
	}

	void CommandList::BeginDebugGroup(const std::string &name)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		BeginDebugGroupCommand command;
		command.GroupName = name;
		m_Commands.push_back(command);

		m_DebugGroups++;
	}

	void CommandList::EndDebugGroup()
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		EndDebugGroupCommand command;
		m_Commands.push_back(command);

		m_DebugGroups--;
	}

	void CommandList::InsertDebugMarker(const std::string &name)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		InsertDebugMarkerCommand command;
		command.MarkerName = name;
		m_Commands.push_back(command);
	}

	void CommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		SetBlendFactorCommand command;
		command.BlendFactorDesc = blendFactor;
		m_Commands.push_back(command);
	}

	void CommandList::SetStencilReference(uint32_t stencilReference)
	{
		SetStencilReferenceCommand command;
		command.StencilReference = stencilReference;
		m_Commands.push_back(command);
	}

	void CommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
		BuildAccelerationStructuresCommand command;
		command.BuildDescriptions = description;
		m_Commands.push_back(command);
	}

	void CommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		m_Commands.push_back(description);
	}

	void CommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		m_Commands.push_back(description);
	}

	void CommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		m_Commands.push_back(description);
	}

	void CommandList::WritePushConstants(const PushConstantsDesc &pushConstantDesc)
	{
	}

	const std::vector<RenderCommandData> &CommandList::GetCommandData() const
	{
		return m_Commands;
	}

	const CommandListDescription &CommandList::GetDescription()
	{
		return m_Description;
	}

	bool CommandList::IsRecording() const
	{
		return m_Started;
	}
}	 // namespace Nexus::Graphics
