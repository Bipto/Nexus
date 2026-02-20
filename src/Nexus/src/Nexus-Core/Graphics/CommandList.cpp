#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Platform/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
	ICommandList::ICommandList(const CommandListDescription &spec) : m_Description(spec), m_AutomaticBarrierTracking(spec.AutomaticBarrierTransitions)
	{
	}

	void ICommandList::Begin()
	{
		NX_PROFILE_FUNCTION();

		if (m_Started)
		{
			NX_ERROR("Attempting to begin a command into a CommandList that has not "
					 "been closed");
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.clear();
		m_Started				   = true;
		m_DebugGroups			   = 0;
		m_AutomaticBarrierTracking = m_Description.AutomaticBarrierTransitions;
		m_Barriers.Clear();
	}

	void ICommandList::End()
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

	void ICommandList::BeginAutomaticBarrierManagement()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_AutomaticBarrierTracking = true;
	}

	void ICommandList::EndAutomaticBarrierManagement()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_AutomaticBarrierTracking = false;
	}

	void ICommandList::SetVertexBuffer(VertexBufferView vertexBuffer, uint32_t slot)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::SetIndexBuffer(IndexBufferView indexBuffer)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::SetPipeline(Ref<Pipeline> pipeline)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(pipeline);
	}

	void ICommandList::Draw(const DrawDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DrawIndexed(const DrawIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DrawIndirect(const DrawIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DrawIndexedIndirect(const DrawIndirectIndexedDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::Dispatch(const DispatchDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DispatchIndirect(const DispatchIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DrawMesh(const DrawMeshDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::DrawMeshIndirect(const DrawMeshIndirectDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::TraceRays(const TraceRaysDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::SetResourceSet(const ResourceSetBindingDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		if (m_AutomaticBarrierTracking)
		{
			const auto &boundResources = desc.TargetResourceSet->GetBoundResources();

			for (const auto &[name, ciSamplers] : boundResources.CombinedImageSamplers)
			{
				for (const CombinedImageSampler &ciSampler : ciSamplers)
				{
					Ref<ITextureView>			  textureView = ciSampler.ImageTexture;
					const TextureViewDescription &viewDesc	  = textureView->GetDescription();

					TextureBarrierDesc barrier					   = {};
					barrier.BeforeAccess						   = BarrierAccess::NoAccess;
					barrier.AfterAccess							   = BarrierAccess::ShaderRead;
					barrier.BeforeStage							   = BarrierPipelineStage::NoStage;
					barrier.AfterStage							   = BarrierPipelineStage::AllGraphics;
					barrier.Texture								   = textureView->GetTexture();
					barrier.Layout								   = TextureLayout::ShaderReadOnlyOptimal;
					barrier.TextureSubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
					barrier.TextureSubresourceRange.LayerCount	   = viewDesc.Range.LayerCount;
					barrier.TextureSubresourceRange.BaseMipLevel   = viewDesc.Range.BaseMipLevel;
					barrier.TextureSubresourceRange.LevelCount	   = viewDesc.Range.LevelCount;
					SubmitTextureBarrier(barrier);
				}
			}

			for (const auto &[name, imageViews] : boundResources.SampledImages)
			{
				for (Ref<ITextureView> imageView : imageViews)
				{
					const TextureViewDescription &viewDesc = imageView->GetDescription();

					TextureBarrierDesc barrier					   = {};
					barrier.BeforeAccess						   = BarrierAccess::NoAccess;
					barrier.AfterAccess							   = BarrierAccess::ShaderRead;
					barrier.BeforeStage							   = BarrierPipelineStage::NoStage;
					barrier.AfterStage							   = BarrierPipelineStage::AllGraphics;
					barrier.Texture								   = imageView->GetTexture();
					barrier.Layout								   = TextureLayout::ShaderReadOnlyOptimal;
					barrier.TextureSubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
					barrier.TextureSubresourceRange.LayerCount	   = viewDesc.Range.LayerCount;
					barrier.TextureSubresourceRange.BaseMipLevel   = viewDesc.Range.BaseMipLevel;
					barrier.TextureSubresourceRange.LevelCount	   = viewDesc.Range.LevelCount;
					SubmitTextureBarrier(barrier);
				}
			}

			for (const auto &[name, storageImages] : boundResources.StorageImages)
			{
				for (const StorageImageView &storageImage : storageImages)
				{
					Ref<ITexture> texture = storageImage.TextureHandle;

					TextureBarrierDesc barrier					   = {};
					barrier.BeforeAccess						   = BarrierAccess::NoAccess;
					barrier.AfterAccess							   = BarrierAccess::ShaderRead;
					barrier.BeforeStage							   = BarrierPipelineStage::NoStage;
					barrier.AfterStage							   = BarrierPipelineStage::AllGraphics;
					barrier.Texture								   = texture;
					barrier.Layout								   = TextureLayout::General;
					barrier.TextureSubresourceRange.BaseArrayLayer = storageImage.ArrayLayer;
					barrier.TextureSubresourceRange.LayerCount	   = 1;
					barrier.TextureSubresourceRange.BaseMipLevel   = storageImage.MipLevel;
					barrier.TextureSubresourceRange.LevelCount	   = 1;
					SubmitTextureBarrier(barrier);
				}
			}
		}

		FlushBarriers();

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void ICommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color, ClearRect clearRect)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColorTargetCommand command;
		command.Index  = index;
		command.Colour = color;
		command.Rect   = clearRect;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::ClearColourTarget(uint32_t index, const ClearColourValue &color)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		ClearColorTargetCommand command;
		command.Index  = index;
		command.Colour = color;
		command.Rect   = std::nullopt;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::ClearDepthTarget(const ClearDepthStencilValue &value, ClearRect clearRect)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	static void TransitionFramebufferLayouts(ICommandList	  *commandList,
											 Ref<IFramebuffer> framebuffer,
											 TextureLayout	   colourLayout,
											 TextureLayout	   depthLayout)
	{
		// transition colour attachment layouts
		for (size_t i = 0; i < framebuffer->GetColorTextureCount(); i++)
		{
			std::optional<FramebufferColourAttachmentDescription> colourAttachmentOpt = framebuffer->GetColorTextureBinding(i);
			if (colourAttachmentOpt.has_value())
			{
				FramebufferColourAttachmentDescription colourAttachment = colourAttachmentOpt.value();

				TextureBarrierDesc barrierDesc		= {};
				barrierDesc.Texture					= colourAttachment.ColourAttachment.TargetTexture;
				barrierDesc.BeforeAccess			= BarrierAccess::NoAccess;
				barrierDesc.AfterAccess				= BarrierAccess::ColourAttachmentWrite;
				barrierDesc.BeforeStage				= BarrierPipelineStage::NoStage;
				barrierDesc.AfterStage				= BarrierPipelineStage::ColourAttachmentOutput;
				barrierDesc.Layout					= colourLayout;
				barrierDesc.TextureSubresourceRange = {.BaseMipLevel   = colourAttachment.ColourAttachment.MipLevel,
													   .LevelCount	   = 1,
													   .BaseArrayLayer = colourAttachment.ColourAttachment.BaseArrayLayer,
													   .LayerCount	   = colourAttachment.ColourAttachment.LayerCount};

				commandList->SubmitTextureBarrier(barrierDesc);

				if (colourAttachment.ResolveAttachment.has_value())
				{
					FramebufferTextureDescription resolveAttachmentDesc = colourAttachment.ResolveAttachment.value();

					TextureBarrierDesc barrierDesc		= {};
					barrierDesc.Texture					= resolveAttachmentDesc.TargetTexture;
					barrierDesc.BeforeAccess			= BarrierAccess::ColourAttachmentWrite;
					barrierDesc.AfterAccess				= BarrierAccess::NoAccess;
					barrierDesc.BeforeStage				= BarrierPipelineStage::ColourAttachmentOutput;
					barrierDesc.AfterStage				= BarrierPipelineStage::AllGraphics;
					barrierDesc.Layout					= colourLayout;
					barrierDesc.TextureSubresourceRange = {.BaseMipLevel   = resolveAttachmentDesc.MipLevel,
														   .LevelCount	   = 1,
														   .BaseArrayLayer = resolveAttachmentDesc.BaseArrayLayer,
														   .LayerCount	   = resolveAttachmentDesc.LayerCount};

					commandList->SubmitTextureBarrier(barrierDesc);
				}
			}
		}

		// transition depth attachment layout if exists
		if (std::optional<FramebufferTextureDescription> depthAttachmentOpt = framebuffer->GetDepthTextureBinding())
		{
			FramebufferTextureDescription depthAttachment = depthAttachmentOpt.value();

			TextureBarrierDesc barrierDesc		= {};
			barrierDesc.Texture					= depthAttachment.TargetTexture;
			barrierDesc.BeforeAccess			= BarrierAccess::NoAccess;
			barrierDesc.AfterAccess				= BarrierAccess::DepthStencilAttachmentWrite;
			barrierDesc.BeforeStage				= BarrierPipelineStage::NoStage;
			barrierDesc.AfterStage				= BarrierPipelineStage::EarlyFragmentTests;
			barrierDesc.Layout					= depthLayout;
			barrierDesc.TextureSubresourceRange = {.BaseMipLevel   = depthAttachment.MipLevel,
												   .LevelCount	   = 1,
												   .BaseArrayLayer = depthAttachment.BaseArrayLayer,
												   .LayerCount	   = depthAttachment.LayerCount};

			commandList->SubmitTextureBarrier(barrierDesc);
		}

		commandList->FlushBarriers();
	}

	void ICommandList::SetFramebuffer(Ref<IFramebuffer> framebuffer)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(framebuffer);
	}

	void ICommandList::SetViewport(const Viewport &viewport)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(viewport);
	}

	void ICommandList::SetScissor(const Scissor &scissor)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(scissor);
	}

	void ICommandList::ResolveFramebuffer(const ResolveTextureDescription &desc)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		if (m_AutomaticBarrierTracking)
		{
			TextureBarrierDesc sourceBarrierDesc	  = {};
			sourceBarrierDesc.Texture				  = desc.Source;
			sourceBarrierDesc.BeforeAccess			  = BarrierAccess::ColourAttachmentWrite;
			sourceBarrierDesc.AfterAccess			  = BarrierAccess::TransferRead;
			sourceBarrierDesc.BeforeStage			  = BarrierPipelineStage::ColourAttachmentOutput;
			sourceBarrierDesc.AfterStage			  = BarrierPipelineStage::Resolve;
			sourceBarrierDesc.Layout				  = TextureLayout::ResolveSrc;
			sourceBarrierDesc.TextureSubresourceRange = {.BaseMipLevel	 = desc.SourceMipLevel,
														 .LevelCount	 = 1,
														 .BaseArrayLayer = desc.SourceArrayLayer,
														 .LayerCount	 = 1};
			SubmitTextureBarrier(sourceBarrierDesc);

			TextureBarrierDesc destBarrierDesc		= {};
			destBarrierDesc.Texture					= desc.Destination;
			destBarrierDesc.BeforeAccess			= BarrierAccess::NoAccess;
			destBarrierDesc.AfterAccess				= BarrierAccess::TransferWrite;
			destBarrierDesc.BeforeStage				= BarrierPipelineStage::NoStage;
			destBarrierDesc.AfterStage				= BarrierPipelineStage::Resolve;
			destBarrierDesc.Layout					= TextureLayout::ResolveDest;
			destBarrierDesc.TextureSubresourceRange = {.BaseMipLevel   = desc.DestinationMipLevel,
													   .LevelCount	   = 1,
													   .BaseArrayLayer = desc.DestinationArrayLayer,
													   .LayerCount	   = 1};
			SubmitTextureBarrier(destBarrierDesc);
		}

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(desc);
	}

	void Nexus::Graphics::ICommandList::StartTimingQuery(Ref<ITimingQuery> query)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void Nexus::Graphics::ICommandList::StopTimingQuery(Ref<ITimingQuery> query)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::CopyBufferToBuffer(const BufferCopyDescription &bufferCopy)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::CopyBufferToTexture(const BufferTextureCopyDescription &bufferTextureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		if (m_AutomaticBarrierTracking)
		{
			Graphics::TextureBarrierDesc barrierDesc = {};
			barrierDesc.Texture						 = bufferTextureCopy.TextureHandle;
			barrierDesc.BeforeAccess				 = BarrierAccess::NoAccess;
			barrierDesc.AfterAccess					 = BarrierAccess::TransferWrite;
			barrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
			barrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
			barrierDesc.Layout						 = TextureLayout::TransferDstOptimal;
			barrierDesc.TextureSubresourceRange		 = {.BaseMipLevel	= bufferTextureCopy.MipLevel,
														.LevelCount		= 1,
														.BaseArrayLayer = (uint32_t)bufferTextureCopy.TextureOffset.Z,
														.LayerCount		= 1};
			SubmitTextureBarrier(barrierDesc);
		}

		FlushBarriers();

		Graphics::CopyBufferToTextureCommand command;
		command.BufferTextureCopy = bufferTextureCopy;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::CopyTextureToBuffer(const BufferTextureCopyDescription &textureBufferCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		if (m_AutomaticBarrierTracking)
		{
			Graphics::TextureBarrierDesc barrierDesc = {};
			barrierDesc.Texture						 = textureBufferCopy.TextureHandle;
			barrierDesc.BeforeAccess				 = BarrierAccess::NoAccess;
			barrierDesc.AfterAccess					 = BarrierAccess::TransferRead;
			barrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
			barrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
			barrierDesc.Layout						 = TextureLayout::TransferSrcOptimal;
			barrierDesc.TextureSubresourceRange		 = {.BaseMipLevel	= textureBufferCopy.MipLevel,
														.LevelCount		= 1,
														.BaseArrayLayer = (uint32_t)textureBufferCopy.TextureOffset.Z,
														.LayerCount		= 1};
			SubmitTextureBarrier(barrierDesc);
		}

		FlushBarriers();

		Graphics::CopyTextureToBufferCommand command;
		command.TextureBufferCopy = textureBufferCopy;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::CopyTextureToTexture(const TextureCopyDescription &textureCopy)
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		if (m_AutomaticBarrierTracking)
		{
			Graphics::TextureBarrierDesc sourceBarrierDesc = {};
			sourceBarrierDesc.Texture					   = textureCopy.Source;
			sourceBarrierDesc.BeforeAccess				   = BarrierAccess::NoAccess;
			sourceBarrierDesc.AfterAccess				   = BarrierAccess::TransferWrite;
			sourceBarrierDesc.BeforeStage				   = BarrierPipelineStage::Copy;
			sourceBarrierDesc.AfterStage				   = BarrierPipelineStage::Copy;
			sourceBarrierDesc.Layout					   = TextureLayout::TransferSrcOptimal;
			sourceBarrierDesc.TextureSubresourceRange	   = {.BaseMipLevel	  = textureCopy.SourceMipLevel,
															  .LevelCount	  = 1,
															  .BaseArrayLayer = (uint32_t)textureCopy.SourceOffset.Z,
															  .LayerCount	  = 1};
			SubmitTextureBarrier(sourceBarrierDesc);

			Graphics::TextureBarrierDesc destBarrierDesc = {};
			destBarrierDesc.Texture						 = textureCopy.Destination;
			destBarrierDesc.BeforeAccess				 = BarrierAccess::NoAccess;
			destBarrierDesc.AfterAccess					 = BarrierAccess::TransferWrite;
			destBarrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
			destBarrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
			destBarrierDesc.Layout						 = TextureLayout::TransferDstOptimal;
			destBarrierDesc.TextureSubresourceRange		 = {.BaseMipLevel	= textureCopy.DestinationMipLevel,
															.LevelCount		= 1,
															.BaseArrayLayer = (uint32_t)textureCopy.DestinationOffset.Z,
															.LayerCount		= 1};
			SubmitTextureBarrier(destBarrierDesc);
		}

		FlushBarriers();

		Graphics::CopyTextureToTextureCommand command;
		command.TextureCopy = textureCopy;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::BeginDebugGroup(const std::string &name)
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
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);

		m_DebugGroups++;
	}

	void ICommandList::EndDebugGroup()
	{
		NX_PROFILE_FUNCTION();

		if (!m_Started)
		{
			NX_ERROR("Attempting to record a command into a CommandList without "
					 "calling Begin()");
			return;
		}

		EndDebugGroupCommand command;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);

		m_DebugGroups--;
	}

	void ICommandList::InsertDebugMarker(const std::string &name)
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		NX_PROFILE_FUNCTION();

		SetBlendFactorCommand command;
		command.BlendFactor = blendFactor;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::SetStencilReference(uint32_t stencilReference)
	{
		NX_PROFILE_FUNCTION();

		SetStencilReferenceCommand command;
		command.StencilReference = stencilReference;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureGeometryBuildDescription> &description)
	{
		NX_PROFILE_FUNCTION();

		BuildAccelerationStructuresCommand command;
		command.BuildDescriptions = description;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(command);
	}

	void ICommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(description);
	}

	void ICommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(description);
	}

	void ICommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(description);
	}

	void ICommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
		NX_PROFILE_FUNCTION();

		std::lock_guard<std::mutex> lock(m_Mutex);

		PushConstantsDesc pushConstantDesc = {};
		pushConstantDesc.Name			   = name;
		pushConstantDesc.Offset			   = offset;
		pushConstantDesc.Data.resize(size);
		memcpy(pushConstantDesc.Data.data(), data, size);
		m_Commands.push_back(pushConstantDesc);
	}

	void ICommandList::SubmitMemoryBarrier(const MemoryBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Barriers.MemoryBarriers.emplace_back(desc);
	}

	void ICommandList::SubmitTextureBarrier(const TextureBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Barriers.TextureBarriers.emplace_back(desc);
	}

	void ICommandList::SubmitBufferBarrier(const BufferBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Barriers.BufferBarriers.emplace_back(desc);
	}

	void ICommandList::FlushBarriers()
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Commands.push_back(m_Barriers);
		m_Barriers.Clear();
	}

	const std::vector<RenderCommandData> &ICommandList::GetCommandData() const
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_Commands;
	}

	const CommandListDescription &ICommandList::GetDescription()
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_Description;
	}

	bool ICommandList::IsRecording() const
	{
		NX_PROFILE_FUNCTION();
		std::lock_guard<std::mutex> lock(m_Mutex);

		return m_Started;
	}

	void ICommandList::EndRendering()
	{
		if (!m_CurrentFramebuffer)
		{
			return;
		}

		EndRenderingCommand command = {};
		command.TargetFramebuffer	= m_CurrentFramebuffer;
		m_Commands.push_back(command);

		if (m_CurrentFramebuffer->IsOwnedBySwapchain())
		{
			TransitionFramebufferLayouts(this, m_CurrentFramebuffer, TextureLayout::PresentSrc, TextureLayout::DepthStencilAttachmentOptimal);
		}

		m_CurrentFramebuffer = nullptr;
	}
}	 // namespace Nexus::Graphics
