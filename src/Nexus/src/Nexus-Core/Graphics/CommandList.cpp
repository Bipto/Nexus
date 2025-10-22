#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
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

		m_Commands.clear();
		m_Started	  = true;
		m_DebugGroups = 0;
	}

	void CommandList::End()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to end a CommandList but the CommandList was not begun");
		}

		// just in case
		EndRendering();

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
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);
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

		m_Commands.push_back(pipeline);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		m_Commands.push_back(desc);
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

		if (m_Description.AutomaticBarrierTransitions)
		{
			const auto &combinedImageSamplers = resources->GetBoundCombinedImageSamplers();
			const auto &storageImages		  = resources->GetBoundStorageImages();
			const auto &storageBuffers		  = resources->GetBoundStorageBuffers();

			for (const auto &[name, ciSampler] : combinedImageSamplers)
			{
				Ref<ITextureView>			  textureView = ciSampler.ImageTexture;
				const TextureViewDescription &viewDesc	  = textureView->GetDescription();

				TextureBarrierDesc barrier				= {};
				barrier.BeforeAccess					= BarrierAccess::None;
				barrier.AfterAccess						= BarrierAccess::ShaderRead;
				barrier.BeforeStage						= BarrierPipelineStage::None;
				barrier.AfterStage						= BarrierPipelineStage::AllGraphics;
				barrier.Texture							= textureView->GetTexture();
				barrier.Layout							= ciSampler.Layout;
				barrier.SubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
				barrier.SubresourceRange.LayerCount		= viewDesc.Range.LayerCount;
				barrier.SubresourceRange.BaseMipLevel	= viewDesc.Range.BaseMipLevel;
				barrier.SubresourceRange.LevelCount		= viewDesc.Range.LevelCount;
				SubmitTextureBarrier(barrier);
			}

			for (const auto &[name, storageImage] : storageImages)
			{
				Ref<Texture> texture = storageImage.TextureHandle;

				TextureBarrierDesc barrier				= {};
				barrier.BeforeAccess					= BarrierAccess::None;
				barrier.AfterAccess						= BarrierAccess::ShaderRead;
				barrier.BeforeStage						= BarrierPipelineStage::None;
				barrier.AfterStage						= BarrierPipelineStage::AllGraphics;
				barrier.Texture							= texture;
				barrier.Layout							= storageImage.Layout;
				barrier.SubresourceRange.BaseArrayLayer = storageImage.ArrayLayer;
				barrier.SubresourceRange.LayerCount		= 1;
				barrier.SubresourceRange.BaseMipLevel	= storageImage.MipLevel;
				barrier.SubresourceRange.LevelCount		= 1;
				SubmitTextureBarrier(barrier);
			}

			for (const auto &[name, storageBuffer] : storageBuffers)
			{
				BufferBarrierDesc barrier = {};
				barrier.AfterAccess		  = BarrierAccess::ShaderWrite;
				barrier.BeforeAccess	  = BarrierAccess::None;
				barrier.AfterStage		  = BarrierPipelineStage::AllGraphics;
				barrier.BeforeStage		  = BarrierPipelineStage::None;
				barrier.Buffer			  = storageBuffer.BufferHandle;
				barrier.Offset			  = storageBuffer.Offset;
				barrier.Size			  = storageBuffer.SizeInBytes;
				SubmitBufferBarrier(barrier);
			}
		}

		m_Commands.push_back(resources);
	}

	void CommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

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

	void CommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color)
	{
		NX_PROFILE_FUNCTION();

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
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);
	}

	void TransitionFramebufferLayouts(CommandList *commandList, Ref<Framebuffer> framebuffer, TextureLayout colourLayout, TextureLayout depthLayout)
	{
		// transition colour attachment layouts
		for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
		{
			std::optional<FramebufferColourAttachmentDescription> colourAttachmentOpt = framebuffer->GetColorTextureBinding(i);
			if (colourAttachmentOpt.has_value())
			{
				FramebufferColourAttachmentDescription colourAttachment = colourAttachmentOpt.value();

				TextureBarrierDesc barrierDesc = {};
				barrierDesc.Texture			   = colourAttachment.ColourAttachment.TargetTexture;
				barrierDesc.BeforeAccess	   = BarrierAccess::None;
				barrierDesc.AfterAccess		   = BarrierAccess::ColourAttachmentWrite;
				barrierDesc.BeforeStage		   = BarrierPipelineStage::None;
				barrierDesc.AfterStage		   = BarrierPipelineStage::ColourAttachmentOutput;
				barrierDesc.Layout			   = colourLayout;
				barrierDesc.SubresourceRange   = {.BaseMipLevel	  = colourAttachment.ColourAttachment.MipLevel,
												  .LevelCount	  = 1,
												  .BaseArrayLayer = colourAttachment.ColourAttachment.BaseArrayLayer,
												  .LayerCount	  = colourAttachment.ColourAttachment.LayerCount};

				commandList->SubmitTextureBarrier(barrierDesc);

				if (colourAttachment.ResolveAttachment.has_value())
				{
					FramebufferTextureDescription resolveAttachmentDesc = colourAttachment.ResolveAttachment.value();

					TextureBarrierDesc barrierDesc = {};
					barrierDesc.Texture			   = resolveAttachmentDesc.TargetTexture;
					barrierDesc.BeforeAccess	   = BarrierAccess::None;
					barrierDesc.AfterAccess		   = BarrierAccess::ColourAttachmentWrite;
					barrierDesc.BeforeStage		   = BarrierPipelineStage::None;
					barrierDesc.AfterStage		   = BarrierPipelineStage::ColourAttachmentOutput;
					barrierDesc.Layout			   = colourLayout;
					barrierDesc.SubresourceRange   = {.BaseMipLevel	  = resolveAttachmentDesc.MipLevel,
													  .LevelCount	  = 1,
													  .BaseArrayLayer = resolveAttachmentDesc.BaseArrayLayer,
													  .LayerCount	  = resolveAttachmentDesc.LayerCount};

					commandList->SubmitTextureBarrier(barrierDesc);
				}
			}
		}

		// transition depth attachment layout if exists
		if (std::optional<FramebufferTextureDescription> depthAttachmentOpt = framebuffer->GetDepthTextureBinding())
		{
			FramebufferTextureDescription depthAttachment = depthAttachmentOpt.value();

			TextureBarrierDesc barrierDesc = {};
			barrierDesc.Texture			   = depthAttachment.TargetTexture;
			barrierDesc.BeforeAccess	   = BarrierAccess::None;
			barrierDesc.AfterAccess		   = BarrierAccess::DepthStencilAttachmentWrite;
			barrierDesc.BeforeStage		   = BarrierPipelineStage::None;
			barrierDesc.AfterStage		   = BarrierPipelineStage::EarlyFragmentTests;
			barrierDesc.Layout			   = depthLayout;
			barrierDesc.SubresourceRange   = {.BaseMipLevel	  = depthAttachment.MipLevel,
											  .LevelCount	  = 1,
											  .BaseArrayLayer = depthAttachment.BaseArrayLayer,
											  .LayerCount	  = depthAttachment.LayerCount};

			commandList->SubmitTextureBarrier(barrierDesc);
		}
	}

	void CommandList::SetFramebuffer(Ref<Framebuffer> framebuffer)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		EndRendering();

		TransitionFramebufferLayouts(this, framebuffer, TextureLayout::ColourAttachmentOptimal, TextureLayout::DepthStencilAttachmentOptimal);
		m_CurrentFramebuffer = framebuffer;

		m_Commands.push_back(framebuffer);
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

		m_Commands.push_back(viewport);
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

		m_Commands.push_back(scissor);
	}

	void CommandList::ResolveFramebuffer(const ResolveTextureDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		m_Commands.push_back(desc);
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
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);
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

		Graphics::TextureBarrierDesc barrierDesc = {};
		barrierDesc.Texture						 = bufferTextureCopy.TextureHandle;
		barrierDesc.BeforeAccess				 = BarrierAccess::None;
		barrierDesc.AfterAccess					 = BarrierAccess::TransferWrite;
		barrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
		barrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
		barrierDesc.Layout						 = TextureLayout::TransferDstOptimal;
		barrierDesc.SubresourceRange			 = {.BaseMipLevel	= bufferTextureCopy.MipLevel,
													.LevelCount		= 1,
													.BaseArrayLayer = (uint32_t)bufferTextureCopy.TextureOffset.Z,
													.LayerCount		= bufferTextureCopy.TextureExtent.Depth};
		SubmitTextureBarrier(barrierDesc);

		Graphics::CopyBufferToTextureCommand command;
		command.BufferTextureCopy = bufferTextureCopy;
		m_Commands.push_back(command);
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

		Graphics::TextureBarrierDesc barrierDesc = {};
		barrierDesc.Texture						 = textureBufferCopy.TextureHandle;
		barrierDesc.BeforeAccess				 = BarrierAccess::None;
		barrierDesc.AfterAccess					 = BarrierAccess::HostRead;
		barrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
		barrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
		barrierDesc.Layout						 = TextureLayout::TransferDstOptimal;
		barrierDesc.SubresourceRange			 = {.BaseMipLevel	= textureBufferCopy.MipLevel,
													.LevelCount		= 1,
													.BaseArrayLayer = (uint32_t)textureBufferCopy.TextureOffset.Z,
													.LayerCount		= textureBufferCopy.TextureExtent.Depth};
		SubmitTextureBarrier(barrierDesc);

		Graphics::CopyTextureToBufferCommand command;
		command.TextureBufferCopy = textureBufferCopy;
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);
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
		m_Commands.push_back(command);

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
		m_Commands.push_back(command);

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
		m_Commands.push_back(command);
	}

	void CommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		NX_PROFILE_FUNCTION();

		SetBlendFactorCommand command;
		command.BlendFactorDesc = blendFactor;
		m_Commands.push_back(command);
	}

	void CommandList::SetStencilReference(uint32_t stencilReference)
	{
		NX_PROFILE_FUNCTION();

		SetStencilReferenceCommand command;
		command.StencilReference = stencilReference;
		m_Commands.push_back(command);
	}

	void CommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
		NX_PROFILE_FUNCTION();

		BuildAccelerationStructuresCommand command;
		command.BuildDescriptions = description;
		m_Commands.push_back(command);
	}

	void CommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void CommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void CommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void CommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
		NX_PROFILE_FUNCTION();

		PushConstantsDesc pushConstantDesc = {};
		pushConstantDesc.Name			   = name;
		pushConstantDesc.Offset			   = offset;
		pushConstantDesc.Data.resize(size);
		memcpy(pushConstantDesc.Data.data(), data, size);
		m_Commands.push_back(pushConstantDesc);
	}

	void CommandList::SubmitMemoryBarrier(const MemoryBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		m_Commands.push_back(desc);
	}

	void CommandList::SubmitTextureBarrier(const TextureBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		m_Commands.push_back(desc);
	}

	void CommandList::SubmitBufferBarrier(const BufferBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		m_Commands.push_back(desc);
	}

	const std::vector<RenderCommandData> &CommandList::GetCommandData() const
	{
		NX_PROFILE_FUNCTION();

		return m_Commands;
	}

	const CommandListDescription &CommandList::GetDescription()
	{
		NX_PROFILE_FUNCTION();

		return m_Description;
	}

	bool CommandList::IsRecording() const
	{
		NX_PROFILE_FUNCTION();

		return m_Started;
	}

	void CommandList::EndRendering()
	{
		if (!m_CurrentFramebuffer)
		{
			return;
		}

		if (m_CurrentFramebuffer->IsOwnedBySwapchain())
		{
			TransitionFramebufferLayouts(this, m_CurrentFramebuffer, TextureLayout::PresentSrc, TextureLayout::DepthStencilAttachmentOptimal);
		}

		EndRenderingCommand command = {};
		command.TargetFramebuffer	= m_CurrentFramebuffer;
		m_Commands.push_back(command);

		m_CurrentFramebuffer = nullptr;
	}
}	 // namespace Nexus::Graphics
