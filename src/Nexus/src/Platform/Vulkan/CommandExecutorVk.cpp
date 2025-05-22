#if defined(NX_PLATFORM_VULKAN)

	#include "CommandExecutorVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "FramebufferVk.hpp"
	#include "PipelineVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "TimingQueryVk.hpp"

namespace Nexus::Graphics
{
	CommandExecutorVk::CommandExecutorVk(GraphicsDeviceVk *device) : m_Device(device)
	{
	}

	CommandExecutorVk::~CommandExecutorVk()
	{
	}

	void CommandExecutorVk::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
	{
		// begin
		{
			// reset command buffer
			{
				vkResetCommandBuffer(m_CommandBuffer, 0);
			}

			// begin command buffer
			{
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType					   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags					   = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to begin command buffer");
				}
			}

			m_CurrentRenderTarget = {};
		}

		// execute commands
		{
			for (const auto &element : commands)
			{
				std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
			}
		}

		// end
		{
			StopRendering();
			vkEndCommandBuffer(m_CommandBuffer);
		}
	}

	void CommandExecutorVk::Reset()
	{
	}

	void CommandExecutorVk::SetCommandBuffer(VkCommandBuffer commandBuffer)
	{
		m_CommandBuffer = commandBuffer;
	}

	void CommandExecutorVk::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> vertexBufferVk	= std::dynamic_pointer_cast<DeviceBufferVk>(command.View.BufferHandle);
		VkBuffer		vertexBuffers[] = {vertexBufferVk->GetVkBuffer()};
		VkDeviceSize	offsets[]		= {command.View.Offset};
		vkCmdBindVertexBuffers(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets);
	}

	void CommandExecutorVk::ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indexBufferVk	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.View.BufferHandle);
		VkBuffer		indexBufferHandle = indexBufferVk->GetVkBuffer();
		VkIndexType		indexType		  = Vk::GetVulkanIndexBufferFormat(command.View.BufferFormat);
		vkCmdBindIndexBuffer(m_CommandBuffer, indexBufferHandle, 0, indexType);
	}

	void CommandExecutorVk::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
		if (command.expired())
		{
			NX_ERROR("Attempting to bind an invalid pipeline");
			return;
		}

		auto vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(command.lock());
		vulkanPipeline->Bind(m_CommandBuffer);

		m_CurrentlyBoundPipeline = command.lock();
	}

	void CommandExecutorVk::ExecuteCommand(DrawCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		vkCmdDraw(m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		vkCmdDrawIndexed(m_CommandBuffer, command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(DrawIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}
		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		vkCmdDrawIndirect(m_CommandBuffer,
						  indirectBuffer->GetVkBuffer(),
						  command.Offset,
						  command.DrawCount,
						  indirectBuffer->GetDescription().StrideInBytes);
	}

	void CommandExecutorVk::ExecuteCommand(DrawIndirectIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}
		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		vkCmdDrawIndexedIndirect(m_CommandBuffer,
								 indirectBuffer->GetVkBuffer(),
								 command.Offset,
								 command.DrawCount,
								 indirectBuffer->GetDescription().StrideInBytes);
	}

	void CommandExecutorVk::ExecuteCommand(DispatchCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		vkCmdDispatch(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorVk::ExecuteCommand(DispatchIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<DeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			vkCmdDispatchIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset);
		}
	}

	void CommandExecutorVk::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		WeakRef<Pipeline> pl = m_CurrentlyBoundPipeline.lock();
		if (auto pipeline = pl.lock())
		{
			auto			resourceSetVk = std::dynamic_pointer_cast<ResourceSetVk>(command);
			Ref<PipelineVk> pipelineVk	  = std::dynamic_pointer_cast<PipelineVk>(pipeline);
			pipelineVk->SetResourceSet(m_CommandBuffer, resourceSetVk);
		}
	}

	void CommandExecutorVk::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index) || !ValidateIsRendering())
		{
			return;
		}

		VkClearAttachment clearAttachment {};
		clearAttachment.aspectMask		 = VK_IMAGE_ASPECT_COLOR_BIT;
		clearAttachment.clearValue.color = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
		clearAttachment.colorAttachment	 = command.Index;

		VkClearRect clearRect;
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount	 = 1;
		clearRect.rect.offset	 = {0, 0};
		clearRect.rect.extent	 = {m_RenderSize};

		if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
		{
			return;
		}

		vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
	}

	void CommandExecutorVk::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearDepth(m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		VkClearAttachment clearAttachment {};
		clearAttachment.aspectMask						= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		clearAttachment.clearValue.depthStencil.depth	= command.Value.Depth;
		clearAttachment.clearValue.depthStencil.stencil = command.Value.Stencil;
		clearAttachment.colorAttachment					= m_DepthAttachmentIndex;

		VkClearRect clearRect;
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount	 = 1;
		clearRect.rect.offset	 = {0, 0};
		clearRect.rect.extent	 = {m_RenderSize};

		if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
		{
			return;
		}

		vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
	}

	void CommandExecutorVk::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		if (m_Rendering)
		{
			vkCmdEndRendering(m_CommandBuffer);
		}

		m_CurrentRenderTarget = command;
		m_RenderSize		  = {m_CurrentRenderTarget.GetSize().X, m_CurrentRenderTarget.GetSize().Y};

		if (m_CurrentRenderTarget.GetType() == RenderTargetType::Swapchain)
		{
			WeakRef<Swapchain> sc = m_CurrentRenderTarget.GetSwapchain();
			if (auto swapchain = sc.lock())
			{
				StartRenderingToSwapchain(swapchain);
			}
		}
		else if (m_CurrentRenderTarget.GetType() == RenderTargetType::Framebuffer)
		{
			WeakRef<Framebuffer> fb = m_CurrentRenderTarget.GetFramebuffer();
			if (auto framebuffer = fb.lock())
			{
				StartRenderingToFramebuffer(framebuffer);
			}
		}
		else
		{
			throw std::runtime_error("Invalid render target type");
		}
	}

	void CommandExecutorVk::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
	{
		if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
		{
			return;
		}

		if (command.Width == 0 || command.Height == 0)
			return;

		VkViewport vp;
		vp.x		= command.X;
		vp.y		= command.Height + command.Y;
		vp.width	= command.Width;
		vp.height	= -command.Height;
		vp.minDepth = command.MinDepth;
		vp.maxDepth = command.MaxDepth;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &vp);
	}

	void CommandExecutorVk::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
	{
		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		VkRect2D rect;
		rect.offset = {(int32_t)command.X, (int32_t)command.Y};
		rect.extent = {(uint32_t)command.Width, (uint32_t)command.Height};
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
	}

	void CommandExecutorVk::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
	{
		if (!ValidateForResolveToSwapchain(command))
		{
			return;
		}

		StopRendering();

		auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(command.Source);
		auto swapchainVk   = std::dynamic_pointer_cast<SwapchainVk>(command.Target);

		VkImage framebufferImage = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImage();
		VkImage swapchainImage	 = swapchainVk->GetColourImage();

		VkImageSubresourceLayers src;
		src.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		src.baseArrayLayer = 0;
		src.layerCount	   = 1;
		src.mipLevel	   = 0;

		VkImageSubresourceLayers dst;
		dst.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		dst.baseArrayLayer = 0;
		dst.layerCount	   = 1;
		dst.mipLevel	   = 0;

		VkImageResolve resolve;
		resolve.dstOffset	   = {0, 0, 0};
		resolve.dstSubresource = dst;
		resolve.extent		   = {framebufferVk->GetFramebufferSpecification().Width, framebufferVk->GetFramebufferSpecification().Height, 1};
		resolve.srcOffset	   = {0, 0, 0};
		resolve.srcSubresource = src;

		auto framebufferLayout = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImageLayout(0, 0);
		auto swapchainLayout   = swapchainVk->GetColorImageLayout();

		vkCmdResolveImage(m_CommandBuffer, framebufferImage, framebufferLayout, swapchainImage, swapchainLayout, 1, &resolve);

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		vkCmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
		vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
	}

	void CommandExecutorVk::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
	}

	void CommandExecutorVk::ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		vkCmdSetStencilReference(m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, command.Value);
	}

	void CommandExecutorVk::ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		vkCmdSetDepthBounds(m_CommandBuffer, command.Min, command.Max);
	}

	void CommandExecutorVk::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		const float blends[] = {command.R, command.G, command.B, command.A};
		vkCmdSetBlendConstants(m_CommandBuffer, blends);
	}

	void CommandExecutorVk::ExecuteCommand(const BarrierDesc &command, GraphicsDevice *device)
	{
		std::vector<VkMemoryBarrier2>		memoryBarriers = {};
		std::vector<VkImageMemoryBarrier2>	imageBarriers  = {};
		std::vector<VkBufferMemoryBarrier2> bufferBarriers = {};

		for (const auto &barrier : command.MemoryBarriers)
		{
			VkPipelineStageFlags2 beforeStage  = Vk::GetBarrierPipelineStage(barrier.BeforeStage);
			VkPipelineStageFlags2 afterStage   = Vk::GetBarrierPipelineStage(barrier.AfterStage);
			VkAccessFlags2		  beforeAccess = Vk::GetBarrierAccessFlags(barrier.BeforeAccess);
			VkAccessFlags2		  afterAccess  = Vk::GetBarrierAccessFlags(barrier.AfterAccess);

			VkMemoryBarrier2 memoryBarrier = {};
			memoryBarrier.sType			   = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
			memoryBarrier.srcStageMask	   = beforeStage;
			memoryBarrier.dstStageMask	   = afterStage;
			memoryBarrier.srcAccessMask	   = beforeAccess;
			memoryBarrier.dstAccessMask	   = afterAccess;
			memoryBarriers.push_back(memoryBarrier);
		}

		for (const auto &barrier : command.TextureBarriers)
		{
			VkPipelineStageFlags2 beforeStage  = Vk::GetBarrierPipelineStage(barrier.BeforeStage);
			VkPipelineStageFlags2 afterStage   = Vk::GetBarrierPipelineStage(barrier.AfterStage);
			VkAccessFlags2		  beforeAccess = Vk::GetBarrierAccessFlags(barrier.BeforeAccess);
			VkAccessFlags2		  afterAccess  = Vk::GetBarrierAccessFlags(barrier.AfterAccess);
			VkImageLayout		  beforeLayout = Vk::GetBarrierLayout(barrier.BeforeLayout);
			VkImageLayout		  afterLayout  = Vk::GetBarrierLayout(barrier.AfterLayout);

			TextureVk *texture = (TextureVk *)barrier.Texture;

			VkImageMemoryBarrier2 imageBarrier			 = {};
			imageBarrier.sType							 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			imageBarrier.srcStageMask					 = beforeStage;
			imageBarrier.dstStageMask					 = afterStage;
			imageBarrier.srcAccessMask					 = beforeAccess;
			imageBarrier.dstAccessMask					 = afterAccess;
			imageBarrier.oldLayout						 = beforeLayout;
			imageBarrier.newLayout						 = afterLayout;
			imageBarrier.srcQueueFamilyIndex			 = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.dstQueueFamilyIndex			 = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.image							 = texture->GetImage();
			imageBarrier.subresourceRange.aspectMask	 = VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageBarrier.subresourceRange.baseMipLevel	 = barrier.SubresourceRange.BaseMipLayer;
			imageBarrier.subresourceRange.levelCount	 = barrier.SubresourceRange.MipLayerCount;
			imageBarrier.subresourceRange.baseArrayLayer = barrier.SubresourceRange.BaseArrayLayer;
			imageBarrier.subresourceRange.layerCount	 = barrier.SubresourceRange.ArrayLayerCount;
			imageBarriers.push_back(imageBarrier);
		}

		for (const auto &barrier : command.BufferBarriers)
		{
			DeviceBufferVk *buffer = (DeviceBufferVk *)barrier.Buffer;

			VkPipelineStageFlags2 beforeStage  = Vk::GetBarrierPipelineStage(barrier.BeforeStage);
			VkPipelineStageFlags2 afterStage   = Vk::GetBarrierPipelineStage(barrier.AfterStage);
			VkAccessFlags2		  beforeAccess = Vk::GetBarrierAccessFlags(barrier.BeforeAccess);
			VkAccessFlags2		  afterAccess  = Vk::GetBarrierAccessFlags(barrier.AfterAccess);

			VkBufferMemoryBarrier2 bufferBarrier = {};
			bufferBarrier.sType					 = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
			bufferBarrier.srcStageMask			 = beforeStage;
			bufferBarrier.dstStageMask			 = afterStage;
			bufferBarrier.srcAccessMask			 = beforeAccess;
			bufferBarrier.dstAccessMask			 = afterAccess;
			bufferBarrier.srcQueueFamilyIndex	 = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.dstQueueFamilyIndex	 = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.buffer				 = buffer->GetVkBuffer();
			bufferBarrier.offset				 = 0;
			bufferBarrier.size					 = buffer->GetDescription().SizeInBytes;
			bufferBarriers.push_back(bufferBarrier);
		}

		VkDependencyInfo dependencyInfo			= {};
		dependencyInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.pNext					= nullptr;
		dependencyInfo.dependencyFlags			= 0;
		dependencyInfo.memoryBarrierCount		= memoryBarriers.size();
		dependencyInfo.pMemoryBarriers			= memoryBarriers.data();
		dependencyInfo.imageMemoryBarrierCount	= imageBarriers.size();
		dependencyInfo.pImageMemoryBarriers		= imageBarriers.data();
		dependencyInfo.bufferMemoryBarrierCount = bufferBarriers.size();
		dependencyInfo.pBufferMemoryBarriers	= bufferBarriers.data();

		vkCmdPipelineBarrier2(m_CommandBuffer, &dependencyInfo);
	}

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferVk> src = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferCopy.Source);
		Ref<DeviceBufferVk> dst = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferCopy.Destination);

		VkBufferCopy bufferCopy = {};
		bufferCopy.srcOffset	= command.BufferCopy.ReadOffset;
		bufferCopy.dstOffset	= command.BufferCopy.WriteOffset;
		bufferCopy.size			= command.BufferCopy.Size;

		vkCmdCopyBuffer(m_CommandBuffer, src->GetVkBuffer(), dst->GetVkBuffer(), 1, &bufferCopy);
	}

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
	{
		GraphicsDeviceVk	 *deviceVk	  = (GraphicsDeviceVk *)device;
		Ref<DeviceBufferVk>	  buffer	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureVk>		  texture	  = std::dynamic_pointer_cast<TextureVk>(command.BufferTextureCopy.TextureHandle);
		VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(command.BufferTextureCopy.TextureSubresource.Aspect);

		std::map<uint32_t, VkImageLayout> previousLayouts;

		for (uint32_t layer = command.BufferTextureCopy.TextureSubresource.ArrayLayer; layer < command.BufferTextureCopy.TextureSubresource.Depth;
			 layer++)
		{
			VkImageLayout layoutBefore = texture->GetImageLayout(layer, command.BufferTextureCopy.TextureSubresource.MipLevel);
			previousLayouts[layer]	   = layoutBefore;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  texture->GetImage(),
												  command.BufferTextureCopy.TextureSubresource.MipLevel,
												  layer,
												  layoutBefore,
												  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
												  VK_IMAGE_ASPECT_COLOR_BIT);
		}

		// perform copy
		{
			VkBufferImageCopy copyRegion		   = {};
			copyRegion.bufferOffset				   = command.BufferTextureCopy.BufferOffset;
			copyRegion.bufferRowLength			   = 0;
			copyRegion.bufferImageHeight		   = 0;
			copyRegion.imageSubresource.aspectMask = aspectFlags;
			copyRegion.imageSubresource.mipLevel   = command.BufferTextureCopy.TextureSubresource.MipLevel;
			copyRegion.imageSubresource.layerCount = command.BufferTextureCopy.TextureSubresource.Depth;
			copyRegion.imageOffset.x			   = command.BufferTextureCopy.TextureSubresource.X;
			copyRegion.imageOffset.y			   = command.BufferTextureCopy.TextureSubresource.Y;
			copyRegion.imageOffset.z				   = 0;
			copyRegion.imageExtent.width		   = command.BufferTextureCopy.TextureSubresource.Width;
			copyRegion.imageExtent.height		   = command.BufferTextureCopy.TextureSubresource.Height;
			copyRegion.imageExtent.depth				   = command.BufferTextureCopy.TextureSubresource.Depth;
			copyRegion.imageSubresource.baseArrayLayer	   = command.BufferTextureCopy.TextureSubresource.ArrayLayer;

			vkCmdCopyBufferToImage(m_CommandBuffer, buffer->GetVkBuffer(), texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
		}

		for (uint32_t layer = command.BufferTextureCopy.TextureSubresource.ArrayLayer; layer < command.BufferTextureCopy.TextureSubresource.Depth;
			 layer++)
		{
			VkImageLayout layoutBefore = texture->GetImageLayout(layer, command.BufferTextureCopy.TextureSubresource.MipLevel);
			previousLayouts[layer]	   = layoutBefore;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  texture->GetImage(),
												  command.BufferTextureCopy.TextureSubresource.MipLevel,
												  layer,
												  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
												  layoutBefore,
												  VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
	{
		GraphicsDeviceVk	 *deviceVk	  = (GraphicsDeviceVk *)device;
		Ref<DeviceBufferVk>	  buffer	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureVk>		  texture	  = std::dynamic_pointer_cast<TextureVk>(command.TextureBufferCopy.TextureHandle);
		VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(command.TextureBufferCopy.TextureSubresource.Aspect);

		std::map<uint32_t, VkImageLayout> previousLayouts;

		for (uint32_t layer = command.TextureBufferCopy.TextureSubresource.ArrayLayer; layer < command.TextureBufferCopy.TextureSubresource.Depth;
			 layer++)
		{
			VkImageLayout layoutBefore = texture->GetImageLayout(layer, command.TextureBufferCopy.TextureSubresource.MipLevel);
			previousLayouts[layer]	   = layoutBefore;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  texture->GetImage(),
												  command.TextureBufferCopy.TextureSubresource.MipLevel,
												  layer,
												  layoutBefore,
												  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
												  VK_IMAGE_ASPECT_COLOR_BIT);
		}

		// perform copy
		{
			VkBufferImageCopy copyRegion		   = {};
			copyRegion.bufferOffset				   = command.TextureBufferCopy.BufferOffset;
			copyRegion.bufferRowLength			   = 0;
			copyRegion.bufferImageHeight		   = 0;
			copyRegion.imageSubresource.aspectMask = aspectFlags;
			copyRegion.imageSubresource.mipLevel   = command.TextureBufferCopy.TextureSubresource.MipLevel;
			copyRegion.imageSubresource.layerCount = command.TextureBufferCopy.TextureSubresource.Depth;
			copyRegion.imageOffset.x			   = command.TextureBufferCopy.TextureSubresource.X;
			copyRegion.imageOffset.y			   = command.TextureBufferCopy.TextureSubresource.Y;
			copyRegion.imageOffset.z				   = command.TextureBufferCopy.TextureSubresource.Z;
			copyRegion.imageExtent.width		   = command.TextureBufferCopy.TextureSubresource.Width;
			copyRegion.imageExtent.height		   = command.TextureBufferCopy.TextureSubresource.Height;
			copyRegion.imageExtent.depth		   = command.TextureBufferCopy.TextureSubresource.Depth;
			copyRegion.imageSubresource.baseArrayLayer = command.TextureBufferCopy.TextureSubresource.ArrayLayer;

			vkCmdCopyImageToBuffer(m_CommandBuffer, texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->GetVkBuffer(), 1, &copyRegion);
		}

		for (uint32_t layer = command.TextureBufferCopy.TextureSubresource.ArrayLayer; layer < command.TextureBufferCopy.TextureSubresource.Depth;
			 layer++)
		{
			VkImageLayout layoutBefore = texture->GetImageLayout(layer, command.TextureBufferCopy.TextureSubresource.MipLevel);
			previousLayouts[layer]	   = layoutBefore;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  texture->GetImage(),
												  command.TextureBufferCopy.TextureSubresource.MipLevel,
												  layer,
												  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
												  layoutBefore,
												  VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)
	{
		GraphicsDeviceVk *deviceVk	 = (GraphicsDeviceVk *)device;
		Ref<TextureVk>	  srcTexture = std::dynamic_pointer_cast<TextureVk>(command.TextureCopy.Source);
		Ref<TextureVk>	  dstTexture = std::dynamic_pointer_cast<TextureVk>(command.TextureCopy.Destination);

		VkImageAspectFlagBits srcAspect = Vk::GetAspectFlags(command.TextureCopy.SourceSubresource.Aspect);
		VkImageAspectFlagBits dstAspect = Vk::GetAspectFlags(command.TextureCopy.DestinationSubresource.Aspect);

		std::map<uint32_t, VkImageLayout> srcLayouts;
		std::map<uint32_t, VkImageLayout> dstLayouts;

		// transfer source texture
		for (uint32_t layer = command.TextureCopy.SourceSubresource.ArrayLayer; layer < command.TextureCopy.SourceSubresource.Depth; layer++)
		{
			VkImageLayout layout = srcTexture->GetImageLayout(layer, command.TextureCopy.SourceSubresource.MipLevel);
			srcLayouts[layer]	 = layout;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  srcTexture->GetImage(),
												  command.TextureCopy.SourceSubresource.MipLevel,
												  layer,
												  layout,
												  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
												  srcAspect);
		}

		// transfer destination texture
		for (uint32_t layer = command.TextureCopy.DestinationSubresource.ArrayLayer; layer < command.TextureCopy.DestinationSubresource.Depth;
			 layer++)
		{
			VkImageLayout layout = dstTexture->GetImageLayout(layer, command.TextureCopy.DestinationSubresource.MipLevel);
			dstLayouts[layer]	 = layout;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  dstTexture->GetImage(),
												  command.TextureCopy.DestinationSubresource.MipLevel,
												  layer,
												  layout,
												  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
												  dstAspect);
		}

		// copy image
		{
			VkImageCopy copyRegion = {};

			// src
			copyRegion.srcSubresource.aspectMask = srcAspect;
			copyRegion.srcSubresource.mipLevel	 = command.TextureCopy.SourceSubresource.MipLevel;
			copyRegion.srcSubresource.layerCount = command.TextureCopy.SourceSubresource.Depth;
			copyRegion.srcOffset.x				 = command.TextureCopy.SourceSubresource.X;
			copyRegion.srcOffset.y				 = command.TextureCopy.SourceSubresource.Y;
			copyRegion.srcOffset.z					 = command.TextureCopy.SourceSubresource.Z;
			copyRegion.srcSubresource.baseArrayLayer = command.TextureCopy.SourceSubresource.ArrayLayer;
			copyRegion.extent.width				 = command.TextureCopy.SourceSubresource.Width;
			copyRegion.extent.height			 = command.TextureCopy.SourceSubresource.Height;
			copyRegion.extent.depth				 = command.TextureCopy.SourceSubresource.Depth;

			// dst
			copyRegion.dstSubresource.aspectMask = dstAspect;
			copyRegion.dstSubresource.mipLevel	 = command.TextureCopy.DestinationSubresource.MipLevel;
			copyRegion.dstSubresource.layerCount = command.TextureCopy.DestinationSubresource.Depth;
			copyRegion.dstOffset.x				 = command.TextureCopy.DestinationSubresource.X;
			copyRegion.dstOffset.y					 = command.TextureCopy.DestinationSubresource.Y;
			copyRegion.dstOffset.z					 = command.TextureCopy.DestinationSubresource.Z;
			copyRegion.dstSubresource.baseArrayLayer = command.TextureCopy.DestinationSubresource.ArrayLayer;

			vkCmdCopyImage(m_CommandBuffer,
						   srcTexture->GetImage(),
						   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						   dstTexture->GetImage(),
						   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						   1,
						   &copyRegion);
		}

		// restore source texture layout
		for (uint32_t layer = command.TextureCopy.SourceSubresource.ArrayLayer; layer < command.TextureCopy.SourceSubresource.Depth; layer++)
		{
			VkImageLayout layout = srcTexture->GetImageLayout(layer, command.TextureCopy.SourceSubresource.MipLevel);
			srcLayouts[layer]	 = layout;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  srcTexture->GetImage(),
												  command.TextureCopy.SourceSubresource.MipLevel,
												  layer,
												  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
												  layout,
												  srcAspect);
		}

		// restore destination texture layout
		for (uint32_t layer = command.TextureCopy.DestinationSubresource.ArrayLayer; layer < command.TextureCopy.DestinationSubresource.Depth;
			 layer++)
		{
			VkImageLayout layout = dstTexture->GetImageLayout(layer, command.TextureCopy.DestinationSubresource.MipLevel);
			dstLayouts[layer]	 = layout;

			deviceVk->TransitionVulkanImageLayout(m_CommandBuffer,
												  dstTexture->GetImage(),
												  command.TextureCopy.DestinationSubresource.MipLevel,
												  layer,
												  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
												  layout,
												  dstAspect);
		}
	}

	void CommandExecutorVk::StartRenderingToSwapchain(Ref<Swapchain> swapchain)
	{
		Ref<SwapchainVk> swapchainVk = std::dynamic_pointer_cast<SwapchainVk>(swapchain);

		m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
											  swapchainVk->GetColourImage(),
											  0,
											  0,
											  swapchainVk->GetColorImageLayout(),
											  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
											  VK_IMAGE_ASPECT_COLOR_BIT);

		m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
											  swapchainVk->GetDepthImage(),
											  0,
											  0,
											  swapchainVk->GetDepthImageLayout(),
											  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
											  VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));

		swapchainVk->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		swapchainVk->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		VkExtent2D swapchainSize = swapchainVk->GetSwapchainSize();

		VkRect2D renderArea;
		renderArea.offset = {0, 0};
		renderArea.extent = swapchainSize;

		VkRenderingAttachmentInfo colourAttachment = {};
		colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;

		if (swapchain->GetSpecification().Samples == 1)
		{
			colourAttachment.imageView	 = swapchainVk->GetColourImageView();
			colourAttachment.imageLayout = swapchainVk->GetColorImageLayout();
		}
		else
		{
			m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
												  swapchainVk->GetResolveImage(),
												  0,
												  0,
												  swapchainVk->GetResolveImageLayout(),
												  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
												  VK_IMAGE_ASPECT_COLOR_BIT);
			swapchainVk->SetResolveImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			colourAttachment.imageView	 = swapchainVk->GetResolveImageView();
			colourAttachment.imageLayout = swapchainVk->GetResolveImageLayout();

			colourAttachment.resolveImageView	= swapchainVk->GetColourImageView();
			colourAttachment.resolveImageLayout = swapchainVk->GetColorImageLayout();
			colourAttachment.resolveMode		= VK_RESOLVE_MODE_AVERAGE_BIT;
		}

		colourAttachment.loadOp		= VK_ATTACHMENT_LOAD_OP_LOAD;
		colourAttachment.storeOp	= VK_ATTACHMENT_STORE_OP_STORE;
		colourAttachment.clearValue = {};

		VkRenderingAttachmentInfo depthAttachment = {};
		depthAttachment.sType					  = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView				  = swapchainVk->GetDepthImageView();
		depthAttachment.imageLayout				  = swapchainVk->GetDepthImageLayout();
		depthAttachment.loadOp					  = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp					  = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.clearValue				  = {};

		VkRenderingInfo renderingInfo	   = {};
		renderingInfo.sType				   = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea		   = renderArea;
		renderingInfo.layerCount		   = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments	   = &colourAttachment;
		renderingInfo.pDepthAttachment	   = &depthAttachment;

		vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
		m_Rendering = true;
	}

	void CommandExecutorVk::StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer)
	{
		Ref<FramebufferVk> vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

		// transition colour layouts
		for (size_t textureIndex = 0; textureIndex < vulkanFramebuffer->GetColorTextureCount(); textureIndex++)
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanColorTexture(textureIndex);

			for (uint32_t mipLevel = 0; mipLevel < texture->GetSpecification().MipLevels; mipLevel++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  mipLevel,
													  0,
													  texture->GetImageLayout(0, mipLevel),
													  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
													  VK_IMAGE_ASPECT_COLOR_BIT);
				texture->SetImageLayout(0, mipLevel, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
		}

		// transition depth layout if needed
		if (vulkanFramebuffer->HasDepthTexture())
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanDepthTexture();

			for (uint32_t mipLevel = 0; mipLevel < texture->GetSpecification().MipLevels; mipLevel++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  mipLevel,
													  0,
													  texture->GetImageLayout(0, mipLevel),
													  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
													  VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
				texture->SetImageLayout(0, mipLevel, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			}
		}

		VkRect2D renderArea {};
		renderArea.offset = {0, 0};
		renderArea.extent = {vulkanFramebuffer->GetFramebufferSpecification().Width, vulkanFramebuffer->GetFramebufferSpecification().Height};

		std::vector<VkRenderingAttachmentInfo> colourAttachments;

		// attach colour textures
		for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < vulkanFramebuffer->GetColorTextureCount(); colourAttachmentIndex++)
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanColorTexture(colourAttachmentIndex);

			VkRenderingAttachmentInfo colourAttachment = {};
			colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colourAttachment.imageView				   = texture->GetImageView();
			colourAttachment.imageLayout			   = texture->GetImageLayout(0, 0);
			colourAttachment.loadOp					   = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp				   = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.clearValue				   = {};
			colourAttachments.push_back(colourAttachment);
		}

		// set up depth attachment (may be unused)
		VkRenderingAttachmentInfo depthAttachment = {};
		if (framebuffer->HasDepthTexture())
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanDepthTexture();

			depthAttachment.sType		= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView	= texture->GetImageView();
			depthAttachment.imageLayout = texture->GetImageLayout(0, 0);
			depthAttachment.loadOp		= VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.clearValue	= {};
		}

		VkRenderingInfo renderingInfo	   = {};
		renderingInfo.sType				   = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea		   = renderArea;
		renderingInfo.layerCount		   = 1;
		renderingInfo.colorAttachmentCount = colourAttachments.size();
		renderingInfo.pColorAttachments	   = colourAttachments.data();

		if (vulkanFramebuffer->HasDepthTexture())
		{
			renderingInfo.pDepthAttachment	 = &depthAttachment;
			renderingInfo.pStencilAttachment = &depthAttachment;
		}
		else
		{
			renderingInfo.pDepthAttachment	 = nullptr;
			renderingInfo.pStencilAttachment = nullptr;
		}

		vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
		m_Rendering = true;
	}

	void CommandExecutorVk::StopRendering()
	{
		if (m_Rendering)
		{
			vkCmdEndRendering(m_CommandBuffer);

			if (m_CurrentRenderTarget.GetType() == RenderTargetType::Framebuffer)
			{
				WeakRef<Framebuffer> fb = m_CurrentRenderTarget.GetFramebuffer();
				if (auto framebuffer = fb.lock())
				{
					TransitionFramebufferToShaderReadonly(framebuffer);
				}
			}
		}

		m_Rendering = false;
	}

	void CommandExecutorVk::TransitionFramebufferToShaderReadonly(Ref<Framebuffer> framebuffer)
	{
		Ref<FramebufferVk> vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

		for (uint32_t colourAttachment = 0; colourAttachment < vulkanFramebuffer->GetColorTextureCount(); colourAttachment++)
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanColorTexture(colourAttachment);

			for (uint32_t level = 0; level < texture->GetSpecification().MipLevels; level++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  level,
													  0,
													  texture->GetImageLayout(0, level),
													  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
													  VK_IMAGE_ASPECT_COLOR_BIT);
				texture->SetImageLayout(0, level, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
		}
	}
	bool CommandExecutorVk::ValidateIsRendering()
	{
		if (!m_Rendering)
		{
			NX_ERROR("Attempting to issue graphics command but rendering has not started");
			return false;
		}
		return true;
	}
}	 // namespace Nexus::Graphics

#endif