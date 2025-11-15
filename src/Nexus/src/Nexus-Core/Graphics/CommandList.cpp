#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

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

		m_Commands.clear();
		m_Started				   = true;
		m_DebugGroups			   = 0;
		m_AutomaticBarrierTracking = m_Description.AutomaticBarrierTransitions;
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
		m_AutomaticBarrierTracking = true;
	}

	void ICommandList::EndAutomaticBarrierManagement()
	{
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

					TextureBarrierDesc barrier				= {};
					barrier.BeforeAccess					= BarrierAccess::None;
					barrier.AfterAccess						= BarrierAccess::ShaderRead;
					barrier.BeforeStage						= BarrierPipelineStage::None;
					barrier.AfterStage						= BarrierPipelineStage::AllGraphics;
					barrier.ITexture						= textureView->GetTexture();
					barrier.Layout							= TextureLayout::ShaderReadOnlyOptimal;
					barrier.SubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
					barrier.SubresourceRange.LayerCount		= viewDesc.Range.LayerCount;
					barrier.SubresourceRange.BaseMipLevel	= viewDesc.Range.BaseMipLevel;
					barrier.SubresourceRange.LevelCount		= viewDesc.Range.LevelCount;
					SubmitTextureBarrier(barrier);
				}
			}

			for (const auto &[name, imageViews] : boundResources.SampledImages)
			{
				for (Ref<ITextureView> imageView : imageViews)
				{
					const TextureViewDescription &viewDesc = imageView->GetDescription();

					TextureBarrierDesc barrier				= {};
					barrier.BeforeAccess					= BarrierAccess::None;
					barrier.AfterAccess						= BarrierAccess::ShaderRead;
					barrier.BeforeStage						= BarrierPipelineStage::None;
					barrier.AfterStage						= BarrierPipelineStage::AllGraphics;
					barrier.ITexture						= imageView->GetTexture();
					barrier.Layout							= TextureLayout::ShaderReadOnlyOptimal;
					barrier.SubresourceRange.BaseArrayLayer = viewDesc.Range.BaseArrayLayer;
					barrier.SubresourceRange.LayerCount		= viewDesc.Range.LayerCount;
					barrier.SubresourceRange.BaseMipLevel	= viewDesc.Range.BaseMipLevel;
					barrier.SubresourceRange.LevelCount		= viewDesc.Range.LevelCount;
					SubmitTextureBarrier(barrier);
				}
			}

			for (const auto &[name, storageImages] : boundResources.StorageImages)
			{
				for (const StorageImageView &storageImage : storageImages)
				{
					Ref<ITexture> texture = storageImage.TextureHandle;

					TextureBarrierDesc barrier				= {};
					barrier.BeforeAccess					= BarrierAccess::None;
					barrier.AfterAccess						= BarrierAccess::ShaderRead;
					barrier.BeforeStage						= BarrierPipelineStage::None;
					barrier.AfterStage						= BarrierPipelineStage::AllGraphics;
					barrier.ITexture						= texture;
					barrier.Layout							= TextureLayout::General;
					barrier.SubresourceRange.BaseArrayLayer = storageImage.ArrayLayer;
					barrier.SubresourceRange.LayerCount		= 1;
					barrier.SubresourceRange.BaseMipLevel	= storageImage.MipLevel;
					barrier.SubresourceRange.LevelCount		= 1;
					SubmitTextureBarrier(barrier);
				}
			}
		}

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
		command.Index = index;
		command.Color = color;
		command.Rect  = clearRect;
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
		command.Index = index;
		command.Color = color;
		command.Rect  = std::nullopt;
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

				TextureBarrierDesc barrierDesc = {};
				barrierDesc.ITexture		   = colourAttachment.ColourAttachment.TargetTexture;
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
					barrierDesc.ITexture		   = resolveAttachmentDesc.TargetTexture;
					barrierDesc.BeforeAccess	   = BarrierAccess::ColourAttachmentWrite;
					barrierDesc.AfterAccess		   = BarrierAccess::None;
					barrierDesc.BeforeStage		   = BarrierPipelineStage::ColourAttachmentOutput;
					barrierDesc.AfterStage		   = BarrierPipelineStage::AllGraphics;
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
			barrierDesc.ITexture		   = depthAttachment.TargetTexture;
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
			TextureBarrierDesc sourceBarrierDesc = {};
			sourceBarrierDesc.ITexture			 = desc.Source;
			sourceBarrierDesc.BeforeAccess		 = BarrierAccess::ColourAttachmentWrite;
			sourceBarrierDesc.AfterAccess		 = BarrierAccess::TransferRead;
			sourceBarrierDesc.BeforeStage		 = BarrierPipelineStage::ColourAttachmentOutput;
			sourceBarrierDesc.AfterStage		 = BarrierPipelineStage::Resolve;
			sourceBarrierDesc.Layout			 = TextureLayout::ResolveSrc;
			sourceBarrierDesc.SubresourceRange	 = {.BaseMipLevel	= desc.SourceMipLevel,
													.LevelCount		= 1,
													.BaseArrayLayer = desc.SourceArrayLayer,
													.LayerCount		= 1};
			SubmitTextureBarrier(sourceBarrierDesc);

			TextureBarrierDesc destBarrierDesc = {};
			destBarrierDesc.ITexture		   = desc.Destination;
			destBarrierDesc.BeforeAccess	   = BarrierAccess::None;
			destBarrierDesc.AfterAccess		   = BarrierAccess::TransferWrite;
			destBarrierDesc.BeforeStage		   = BarrierPipelineStage::None;
			destBarrierDesc.AfterStage		   = BarrierPipelineStage::Resolve;
			destBarrierDesc.Layout			   = TextureLayout::ResolveDest;
			destBarrierDesc.SubresourceRange   = {.BaseMipLevel	  = desc.DestinationMipLevel,
												  .LevelCount	  = 1,
												  .BaseArrayLayer = desc.DestinationArrayLayer,
												  .LayerCount	  = 1};
			SubmitTextureBarrier(destBarrierDesc);
		}

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
			barrierDesc.ITexture					 = bufferTextureCopy.TextureHandle;
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
		}

		Graphics::CopyBufferToTextureCommand command;
		command.BufferTextureCopy = bufferTextureCopy;
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
			barrierDesc.ITexture					 = textureBufferCopy.TextureHandle;
			barrierDesc.BeforeAccess				 = BarrierAccess::None;
			barrierDesc.AfterAccess					 = BarrierAccess::TransferRead;
			barrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
			barrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
			barrierDesc.Layout						 = TextureLayout::TransferSrcOptimal;
			barrierDesc.SubresourceRange			 = {.BaseMipLevel	= textureBufferCopy.MipLevel,
														.LevelCount		= 1,
														.BaseArrayLayer = (uint32_t)textureBufferCopy.TextureOffset.Z,
														.LayerCount		= textureBufferCopy.TextureExtent.Depth};
			SubmitTextureBarrier(barrierDesc);
		}

		Graphics::CopyTextureToBufferCommand command;
		command.TextureBufferCopy = textureBufferCopy;
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
			sourceBarrierDesc.ITexture					   = textureCopy.Source;
			sourceBarrierDesc.BeforeAccess				   = BarrierAccess::None;
			sourceBarrierDesc.AfterAccess				   = BarrierAccess::TransferWrite;
			sourceBarrierDesc.BeforeStage				   = BarrierPipelineStage::Copy;
			sourceBarrierDesc.AfterStage				   = BarrierPipelineStage::Copy;
			sourceBarrierDesc.Layout					   = TextureLayout::TransferSrcOptimal;
			sourceBarrierDesc.SubresourceRange			   = {.BaseMipLevel	  = textureCopy.SourceMipLevel,
															  .LevelCount	  = 1,
															  .BaseArrayLayer = (uint32_t)textureCopy.SourceOffset.Z,
															  .LayerCount	  = textureCopy.Extent.Depth};
			SubmitTextureBarrier(sourceBarrierDesc);

			Graphics::TextureBarrierDesc destBarrierDesc = {};
			destBarrierDesc.ITexture					 = textureCopy.Destination;
			destBarrierDesc.BeforeAccess				 = BarrierAccess::None;
			destBarrierDesc.AfterAccess					 = BarrierAccess::TransferWrite;
			destBarrierDesc.BeforeStage					 = BarrierPipelineStage::Copy;
			destBarrierDesc.AfterStage					 = BarrierPipelineStage::Copy;
			destBarrierDesc.Layout						 = TextureLayout::TransferDstOptimal;
			destBarrierDesc.SubresourceRange			 = {.BaseMipLevel	= textureCopy.DestinationMipLevel,
															.LevelCount		= 1,
															.BaseArrayLayer = (uint32_t)textureCopy.DestinationOffset.Z,
															.LayerCount		= textureCopy.Extent.Depth};
			SubmitTextureBarrier(destBarrierDesc);
		}

		Graphics::CopyTextureToTextureCommand command;
		command.TextureCopy = textureCopy;
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
		m_Commands.push_back(command);
	}

	void ICommandList::SetBlendFactor(const BlendFactorDesc &blendFactor)
	{
		NX_PROFILE_FUNCTION();

		SetBlendFactorCommand command;
		command.BlendFactorDesc = blendFactor;
		m_Commands.push_back(command);
	}

	void ICommandList::SetStencilReference(uint32_t stencilReference)
	{
		NX_PROFILE_FUNCTION();

		SetStencilReferenceCommand command;
		command.StencilReference = stencilReference;
		m_Commands.push_back(command);
	}

	void ICommandList::BuildAccelerationStructures(const std::vector<AccelerationStructureBuildDescription> &description)
	{
		NX_PROFILE_FUNCTION();

		BuildAccelerationStructuresCommand command;
		command.BuildDescriptions = description;
		m_Commands.push_back(command);
	}

	void ICommandList::CopyAccelerationStructure(const AccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void ICommandList::CopyAccelerationStructureToDeviceBuffer(const AccelerationStructureDeviceBufferCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void ICommandList::CopyDeviceBufferToAccelerationStructure(const DeviceBufferAccelerationStructureCopyDescription &description)
	{
		NX_PROFILE_FUNCTION();

		m_Commands.push_back(description);
	}

	void ICommandList::WritePushConstants(const std::string &name, const void *data, size_t size, size_t offset)
	{
		NX_PROFILE_FUNCTION();

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
		m_Commands.push_back(desc);
	}

	void ICommandList::SubmitTextureBarrier(const TextureBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		m_Commands.push_back(desc);
	}

	void ICommandList::SubmitBufferBarrier(const BufferBarrierDesc &desc)
	{
		NX_PROFILE_FUNCTION();
		m_Commands.push_back(desc);
	}

	const std::vector<RenderCommandData> &ICommandList::GetCommandData() const
	{
		NX_PROFILE_FUNCTION();

		return m_Commands;
	}

	const CommandListDescription &ICommandList::GetDescription()
	{
		NX_PROFILE_FUNCTION();

		return m_Description;
	}

	bool ICommandList::IsRecording() const
	{
		NX_PROFILE_FUNCTION();

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
