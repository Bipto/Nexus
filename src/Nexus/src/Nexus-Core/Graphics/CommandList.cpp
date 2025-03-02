#include "Nexus-Core/Graphics/CommandList.hpp"

namespace Nexus::Graphics
{
	CommandList::CommandList(const CommandListSpecification &spec) : m_Specification(spec)
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
		m_Started = true;
	}

	void CommandList::End()
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to end a CommandList but the CommandList was not begun");
		}

		m_Started = false;
	}

	void CommandList::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetVertexBufferCommand command;
		command.VertexBufferRef = vertexBuffer;
		command.Slot			= slot;
		m_Commands.push_back(command);
	}

	void CommandList::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(indexBuffer);
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

	void CommandList::Draw(uint32_t start, uint32_t count)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawElementCommand command;
		command.Start = start;
		command.Count = count;
		m_Commands.push_back(command);
	}

	void CommandList::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawIndexedCommand command;
		command.VertexStart = vertexStart;
		command.IndexStart	= indexStart;
		command.Count		= count;
		m_Commands.push_back(command);
	}

	void CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawInstancedCommand command;
		command.VertexCount	  = vertexCount;
		command.InstanceCount = instanceCount;
		command.VertexStart	  = vertexStart;
		command.InstanceStart = instanceStart;
		m_Commands.push_back(command);
	}

	void CommandList::DrawInstancedIndexed(uint32_t indexCount,
										   uint32_t instanceCount,
										   uint32_t vertexStart,
										   uint32_t indexStart,
										   uint32_t instanceStart)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		DrawInstancedIndexedCommand command;
		command.IndexCount	  = indexCount;
		command.InstanceCount = instanceCount;
		command.VertexStart	  = vertexStart;
		command.IndexStart	  = indexStart;
		command.InstanceStart = instanceStart;
		m_Commands.push_back(command);
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

	void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
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

	void CommandList::SetStencilRef(uint32_t stencil)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetStencilRefCommand command;
		command.Value = stencil;
		m_Commands.push_back(command);
	}

	void CommandList::SetDepthBounds(float min, float max)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetDepthBoundsCommand command;
		command.Min = min;
		command.Max = max;
		m_Commands.push_back(command);
	}

	void CommandList::SetBlendFactor(float r, float g, float b, float a)
	{
		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		SetBlendFactorCommand command;
		command.R = r;
		command.G = g;
		command.B = b;
		command.A = a;
		m_Commands.push_back(command);
	}

	const std::vector<RenderCommandData> &CommandList::GetCommandData() const
	{
		return m_Commands;
	}

	const CommandListSpecification &CommandList::GetSpecification()
	{
		return m_Specification;
	}
}	 // namespace Nexus::Graphics
