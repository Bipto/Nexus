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

	void CommandExecutorVk::ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		// begin
		{
			// reset command buffer
			{
				context.ResetCommandBuffer(m_CommandBuffer, 0);
			}

			// begin command buffer
			{
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType					   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags					   = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				if (context.BeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to begin command buffer");
				}
			}

			m_CurrentRenderTarget = {};
		}

		// execute commands
		{
			const std::vector<RenderCommandData> &commands = commandList->GetCommandData();
			m_Commands									   = commands;
			for (m_CurrentCommandIndex = 0; m_CurrentCommandIndex < commands.size(); m_CurrentCommandIndex++)
			{
				const auto &element = commands.at(m_CurrentCommandIndex);
				std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
			}
			m_Commands.clear();
		}

		// end
		{
			StopRendering();
			context.EndCommandBuffer(m_CommandBuffer);
		}
	}

	void CommandExecutorVk::Reset()
	{
	}

	void CommandExecutorVk::SetCommandBuffer(VkCommandBuffer commandBuffer)
	{
		m_CommandBuffer = commandBuffer;
	}

	void CommandExecutorVk::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> vertexBufferVk	= std::dynamic_pointer_cast<DeviceBufferVk>(command.View.BufferHandle);
		VkBuffer			vertexBuffers[] = {vertexBufferVk->GetVkBuffer()};
		VkDeviceSize		offsets[]		= {command.View.Offset};
		VkDeviceSize		sizes[]			= {command.View.Size};

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdBindVertexBuffers2)
		{
			context.CmdBindVertexBuffers2(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets, sizes, nullptr);
		}
		else if (context.CmdBindVertexBuffers2EXT)
		{
			context.CmdBindVertexBuffers2EXT(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets, sizes, nullptr);
		}
		else
		{
			context.CmdBindVertexBuffers(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indexBufferVk	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.View.BufferHandle);
		VkBuffer			indexBufferHandle = indexBufferVk->GetVkBuffer();
		VkIndexType			indexType		  = Vk::GetVulkanIndexBufferFormat(command.View.BufferFormat);
		VkDeviceSize		offset			  = command.View.Offset;
		VkDeviceSize		size			  = command.View.Size;

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdBindIndexBuffer2)
		{
			context.CmdBindIndexBuffer2(m_CommandBuffer, indexBufferHandle, offset, size, indexType);
		}
		else if (context.CmdBindIndexBuffer2KHR)
		{
			context.CmdBindIndexBuffer2KHR(m_CommandBuffer, indexBufferHandle, offset, size, indexType);
		}
		else
		{
			context.CmdBindIndexBuffer(m_CommandBuffer, indexBufferHandle, offset, indexType);
		}
	}

	void CommandExecutorVk::ExecuteCommand(WeakRef<Pipeline> command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (command.expired())
		{
			NX_ERROR("Attempting to bind an invalid pipeline");
			return;
		}

		if (Ref<Pipeline> pipeline = command.lock())
		{
			m_CurrentlyBoundPipeline   = pipeline;
			Ref<PipelineVk> pipelineVk = std::dynamic_pointer_cast<PipelineVk>(pipeline);

			if (pipeline->GetType() != PipelineType::Graphics && pipeline->GetType() != PipelineType::Meshlet)
			{
				pipelineVk->Bind(m_CommandBuffer, VK_NULL_HANDLE);
			}
			else
			{
				// we immediately bind the graphics/meshlet pipeline if dynamic rendering is available, otherwise we need to know which VkRenderPass
				// to use with it
				GraphicsDeviceVk		   *deviceVk	   = (GraphicsDeviceVk *)device;
				const VulkanDeviceFeatures &deviceFeatures = deviceVk->GetDeviceFeatures();
				if (deviceFeatures.DynamicRenderingAvailable)
				{
					pipelineVk->Bind(m_CommandBuffer, VK_NULL_HANDLE);
				}
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDraw(m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDrawIndexed(m_CommandBuffer,
							   command.IndexCount,
							   command.InstanceCount,
							   command.IndexStart,
							   command.VertexStart,
							   command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDrawIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDrawIndexedIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDispatch(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<IDeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferVk>		 indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			const GladVulkanContext &context		= m_Device->GetVulkanContext();
			context.CmdDispatchIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdDrawMeshTasksEXT)
		{
			context.CmdDrawMeshTasksEXT(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdDrawMeshTasksIndirectEXT)
		{
			context.CmdDrawMeshTasksIndirectEXT(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
	{
		TryStartRendering();

		WeakRef<Pipeline> pl = m_CurrentlyBoundPipeline.lock();
		if (auto pipeline = pl.lock())
		{
			Ref<PipelineVk> pipelineVk = std::dynamic_pointer_cast<PipelineVk>(pipeline);
			pipelineVk->SetResourceSet(m_CommandBuffer, desc);

			Ref<ResourceSetVk> resourceSet = std::dynamic_pointer_cast<ResourceSetVk>(desc.TargetResourceSet);
			m_CurrentlyBoundResourceSet	   = resourceSet;
		}
	}

	void CommandExecutorVk::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
	{
		TryStartRendering();

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

		if (command.Rect.has_value())
		{
			Graphics::ClearRect rect = command.Rect.value();
			clearRect.rect.offset	 = {rect.X, rect.Y};
			clearRect.rect.extent	 = {rect.Width, rect.Height};
		}
		else
		{
			clearRect.rect.offset = {0, 0};
			clearRect.rect.extent = {m_RenderSize};
		}

		if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
		{
			return;
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
	}

	void CommandExecutorVk::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
	{
		TryStartRendering();

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

		if (command.Rect.has_value())
		{
			Graphics::ClearRect rect = command.Rect.value();
			clearRect.rect.offset	 = {rect.X, rect.Y};
			clearRect.rect.extent	 = {rect.Width, rect.Height};
		}
		else
		{
			clearRect.rect.offset = {0, 0};
			clearRect.rect.extent = {m_RenderSize};
		}

		if (clearRect.rect.extent.width == 0 || clearRect.rect.extent.height == 0)
		{
			return;
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
	}

	void CommandExecutorVk::ExecuteCommand(WeakRef<IFramebuffer> command, IGraphicsDevice *device)
	{
		StopRendering();

		if (auto framebuffer = command.lock())
		{
			StartRenderingToFramebuffer(framebuffer);
			m_CurrentRenderTarget = framebuffer;
			m_RenderSize		  = {framebuffer->GetWidth(), framebuffer->GetHeight()};
		}
	}

	void CommandExecutorVk::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
	{
		TryStartRendering();

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

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetViewport(m_CommandBuffer, 0, 1, &vp);
	}

	void CommandExecutorVk::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
	{
		TryStartRendering();

		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		VkRect2D rect;
		rect.offset = {(int32_t)command.X, (int32_t)command.Y};
		rect.extent = {(uint32_t)command.Width, (uint32_t)command.Height};

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetScissor(m_CommandBuffer, 0, 1, &rect);
	}

	void CommandExecutorVk::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForResolve(command))
		{
			return;
		}

		StopRendering();

		auto source		 = std::dynamic_pointer_cast<TextureVk>(command.Source);
		auto destination = std::dynamic_pointer_cast<TextureVk>(command.Destination);

		Point2D<uint32_t> size = Utils::GetMipSize(command.Source->GetWidth(), command.Source->GetHeight(), command.SourceMipLevel);

		VkImageSubresourceLayers srcSubresource = {};
		srcSubresource.aspectMask	  = source->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		srcSubresource.mipLevel		  = command.SourceMipLevel;
		srcSubresource.baseArrayLayer = 0;
		srcSubresource.layerCount	  = 1;

		if (source->GetType() != TextureType::Texture3D)
		{
			srcSubresource.baseArrayLayer = command.SourceArrayLayer;
		}

		VkOffset3D srcOffset = {};
		srcOffset.x			 = 0;
		srcOffset.y			 = 0;
		srcOffset.z			 = 0;

		if (source->GetType() == TextureType::Texture3D)
		{
			srcOffset.z = command.SourceArrayLayer;
		}

		VkImageSubresourceLayers dstSubresource = {};
		dstSubresource.aspectMask	  = destination->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		dstSubresource.mipLevel		  = command.DestinationMipLevel;
		dstSubresource.baseArrayLayer = 0;
		dstSubresource.layerCount	  = 1;

		if (destination->GetType() != TextureType::Texture3D)
		{
			dstSubresource.baseArrayLayer = command.DestinationArrayLayer;
		}

		VkOffset3D dstOffset = {};
		dstOffset.x			 = 0;
		dstOffset.y			 = 0;
		dstOffset.z			 = 0;

		if (destination->GetType() == TextureType::Texture3D)
		{
			dstOffset.z = command.DestinationArrayLayer;
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdResolveImage2KHR)
		{
			VkImageResolve2KHR resolve = {};
			resolve.sType			   = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2_KHR;
			resolve.pNext			   = nullptr;
			resolve.srcOffset		   = srcOffset;
			resolve.dstOffset		   = dstOffset;
			resolve.extent			   = {size.X, size.Y, 1};
			resolve.srcSubresource	   = srcSubresource;
			resolve.dstSubresource	   = dstSubresource;

			VkResolveImageInfo2KHR resolveInfo = {};
			resolveInfo.sType				   = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2_KHR;
			resolveInfo.pNext				   = nullptr;
			resolveInfo.srcImage			   = source->GetImage();
			resolveInfo.srcImageLayout		   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			resolveInfo.dstImage			   = destination->GetImage();
			resolveInfo.dstImageLayout		   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			resolveInfo.regionCount			   = 1;
			resolveInfo.pRegions			   = &resolve;

			context.CmdResolveImage2KHR(m_CommandBuffer, &resolveInfo);
		}
		else
		{
			VkImageResolve resolve = {};
			resolve.srcOffset	   = srcOffset;
			resolve.dstOffset	   = dstOffset;
			resolve.extent		   = {size.X, size.Y, 1};
			resolve.srcSubresource = srcSubresource;
			resolve.dstSubresource = dstSubresource;

			context.CmdResolveImage(m_CommandBuffer,
									source->GetImage(),
									VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
									destination->GetImage(),
									VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									1,
									&resolve);
		}

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
	{
		Ref<TimingQueryVk>		 queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		context.CmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
		context.CmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
	}

	void CommandExecutorVk::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
	{
		Ref<TimingQueryVk>		 queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		context.CmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
	}

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferVk> src = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferCopy.Source);
		Ref<DeviceBufferVk> dst = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferCopy.Destination);

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdCopyBuffer2KHR)
		{
			std::vector<VkBufferCopy2KHR> bufferCopies;

			for (const auto &copy : command.BufferCopy.Copies)
			{
				VkBufferCopy2KHR &bufferCopy = bufferCopies.emplace_back();
				bufferCopy.sType			 = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR;
				bufferCopy.pNext			 = nullptr;
				bufferCopy.srcOffset		 = copy.ReadOffset;
				bufferCopy.dstOffset		 = copy.WriteOffset;
				bufferCopy.size				 = copy.Size;
			}

			VkCopyBufferInfo2KHR copyInfo = {};
			copyInfo.sType				  = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR;
			copyInfo.pNext				  = nullptr;
			copyInfo.srcBuffer			  = src->GetVkBuffer();
			copyInfo.dstBuffer			  = dst->GetVkBuffer();
			copyInfo.regionCount		  = bufferCopies.size();
			copyInfo.pRegions			  = bufferCopies.data();

			context.CmdCopyBuffer2KHR(m_CommandBuffer, &copyInfo);
		}
		else
		{
			std::vector<VkBufferCopy> bufferCopies;

			for (const auto &copy : command.BufferCopy.Copies)
			{
				VkBufferCopy &bufferCopy = bufferCopies.emplace_back();
				bufferCopy.srcOffset	 = copy.ReadOffset;
				bufferCopy.dstOffset	 = copy.WriteOffset;
				bufferCopy.size			 = copy.Size;
			}

			context.CmdCopyBuffer(m_CommandBuffer, src->GetVkBuffer(), dst->GetVkBuffer(), bufferCopies.size(), bufferCopies.data());
		}
	}

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceVk	 *deviceVk	  = (GraphicsDeviceVk *)device;
		Ref<DeviceBufferVk>	  buffer	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureVk>		  texture	  = std::dynamic_pointer_cast<TextureVk>(command.BufferTextureCopy.TextureHandle);
		VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

		std::map<uint32_t, VkImageLayout> previousLayouts;

		// perform copy
		{
			const GladVulkanContext &context = m_Device->GetVulkanContext();

			VkImageSubresourceLayers imageSubresource = {};
			imageSubresource.aspectMask				  = aspectFlags;
			imageSubresource.mipLevel				  = command.BufferTextureCopy.MipLevel;
			imageSubresource.baseArrayLayer			  = 0;
			imageSubresource.layerCount				  = 1;

			if (texture->GetType() != TextureType::Texture3D)
			{
				imageSubresource.baseArrayLayer = command.BufferTextureCopy.TextureOffset.Z;
				imageSubresource.layerCount		= command.BufferTextureCopy.TextureExtent.Depth;
			}

			VkOffset3D imageOffset = {};
			imageOffset.x		   = command.BufferTextureCopy.TextureOffset.X;
			imageOffset.y		   = command.BufferTextureCopy.TextureOffset.Y;
			imageOffset.z		   = 0;

			if (texture->GetType() == TextureType::Texture3D)
			{
				imageOffset.z = command.BufferTextureCopy.TextureOffset.Z;
			}

			VkExtent3D imageExtent = {};
			imageExtent.width	   = command.BufferTextureCopy.TextureExtent.Width;
			imageExtent.height	   = command.BufferTextureCopy.TextureExtent.Height;
			imageExtent.depth	   = command.BufferTextureCopy.TextureExtent.Depth;

			if (context.CmdCopyBufferToImage2KHR)
			{
				VkBufferImageCopy2KHR copyRegion = {};
				copyRegion.sType				 = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
				copyRegion.pNext				 = nullptr;
				copyRegion.bufferOffset			 = command.BufferTextureCopy.BufferOffset;
				copyRegion.bufferRowLength		 = command.BufferTextureCopy.BufferRowLength;
				copyRegion.bufferImageHeight	 = command.BufferTextureCopy.BufferImageHeight;
				copyRegion.imageSubresource		 = imageSubresource;
				copyRegion.imageOffset			 = imageOffset;
				copyRegion.imageExtent			 = imageExtent;

				VkCopyBufferToImageInfo2KHR copyInfo = {};
				copyInfo.sType						 = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR;
				copyInfo.pNext						 = nullptr;
				copyInfo.srcBuffer					 = buffer->GetVkBuffer();
				copyInfo.dstImage					 = texture->GetImage();
				copyInfo.dstImageLayout				 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				copyInfo.pRegions					 = &copyRegion;
				copyInfo.regionCount				 = 1;

				context.CmdCopyBufferToImage2KHR(m_CommandBuffer, &copyInfo);
			}
			else
			{
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset		 = command.BufferTextureCopy.BufferOffset;
				copyRegion.bufferRowLength	 = command.BufferTextureCopy.BufferRowLength;
				copyRegion.bufferImageHeight = command.BufferTextureCopy.BufferImageHeight;
				copyRegion.imageSubresource	 = imageSubresource;
				copyRegion.imageOffset		 = imageOffset;
				copyRegion.imageExtent		 = imageExtent;

				context.CmdCopyBufferToImage(m_CommandBuffer,
											 buffer->GetVkBuffer(),
											 texture->GetImage(),
											 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											 1,
											 &copyRegion);
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceVk	 *deviceVk	  = (GraphicsDeviceVk *)device;
		Ref<DeviceBufferVk>	  buffer	  = std::dynamic_pointer_cast<DeviceBufferVk>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureVk>		  texture	  = std::dynamic_pointer_cast<TextureVk>(command.TextureBufferCopy.TextureHandle);
		VkImageAspectFlagBits aspectFlags = Vk::GetAspectFlags(texture->IsDepth());

		std::map<uint32_t, VkImageLayout> previousLayouts;

		// perform copy
		{
			const GladVulkanContext &context = m_Device->GetVulkanContext();

			VkImageSubresourceLayers imageSubresource = {};
			imageSubresource.aspectMask				  = aspectFlags;
			imageSubresource.mipLevel				  = command.TextureBufferCopy.MipLevel;
			imageSubresource.baseArrayLayer			  = 0;
			imageSubresource.layerCount				  = 1;

			if (texture->GetType() != TextureType::Texture3D)
			{
				imageSubresource.baseArrayLayer = command.TextureBufferCopy.TextureOffset.Z;
				imageSubresource.layerCount		= command.TextureBufferCopy.TextureExtent.Depth;
			}

			VkOffset3D imageOffset = {};
			imageOffset.x		   = command.TextureBufferCopy.TextureOffset.X;
			imageOffset.y		   = command.TextureBufferCopy.TextureOffset.Y;
			imageOffset.z		   = 0;

			if (texture->GetType() == TextureType::Texture3D)
			{
				imageOffset.z = command.TextureBufferCopy.TextureOffset.Z;
			}

			VkExtent3D imageExtent = {};
			imageExtent.width	   = command.TextureBufferCopy.TextureExtent.Width;
			imageExtent.height	   = command.TextureBufferCopy.TextureExtent.Height;
			imageExtent.depth	   = command.TextureBufferCopy.TextureExtent.Depth;

			if (context.CmdCopyImageToBuffer2KHR)
			{
				VkBufferImageCopy2KHR copyRegion = {};
				copyRegion.sType				 = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
				copyRegion.pNext				 = nullptr;
				copyRegion.bufferOffset			 = command.TextureBufferCopy.BufferOffset;
				copyRegion.bufferRowLength		 = command.TextureBufferCopy.BufferRowLength;
				copyRegion.bufferImageHeight	 = command.TextureBufferCopy.BufferImageHeight;
				copyRegion.imageSubresource		 = imageSubresource;
				copyRegion.imageOffset			 = imageOffset;
				copyRegion.imageExtent			 = imageExtent;

				VkCopyImageToBufferInfo2KHR copyInfo = {};
				copyInfo.sType						 = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR;
				copyInfo.pNext						 = nullptr;
				copyInfo.srcImage					 = texture->GetImage();
				copyInfo.srcImageLayout				 = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				copyInfo.dstBuffer					 = buffer->GetVkBuffer();
				copyInfo.regionCount				 = 1;
				copyInfo.pRegions					 = &copyRegion;

				context.CmdCopyImageToBuffer2KHR(m_CommandBuffer, &copyInfo);
			}
			else
			{
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset		 = command.TextureBufferCopy.BufferOffset;
				copyRegion.bufferRowLength	 = command.TextureBufferCopy.BufferRowLength;
				copyRegion.bufferImageHeight = command.TextureBufferCopy.BufferImageHeight;
				copyRegion.imageSubresource	 = imageSubresource;
				copyRegion.imageOffset		 = imageOffset;
				copyRegion.imageExtent		 = imageExtent;

				context.CmdCopyImageToBuffer(m_CommandBuffer,
											 texture->GetImage(),
											 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											 buffer->GetVkBuffer(),
											 1,
											 &copyRegion);
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceVk *deviceVk	 = (GraphicsDeviceVk *)device;
		Ref<TextureVk>	  srcTexture = std::dynamic_pointer_cast<TextureVk>(command.TextureCopy.Source);
		Ref<TextureVk>	  dstTexture = std::dynamic_pointer_cast<TextureVk>(command.TextureCopy.Destination);

		VkImageAspectFlagBits srcAspect = Vk::GetAspectFlags(srcTexture->IsDepth());
		VkImageAspectFlagBits dstAspect = Vk::GetAspectFlags(dstTexture->IsDepth());

		std::map<uint32_t, VkImageLayout> srcLayouts;
		std::map<uint32_t, VkImageLayout> dstLayouts;

		// copy image
		{
			VkImageSubresourceLayers srcSubresource;
			srcSubresource.aspectMask	  = srcAspect;
			srcSubresource.mipLevel		  = command.TextureCopy.SourceMipLevel;
			srcSubresource.baseArrayLayer = 0;
			srcSubresource.layerCount	  = 1;

			if (srcTexture->GetType() != TextureType::Texture3D)
			{
				srcSubresource.baseArrayLayer = command.TextureCopy.SourceOffset.Z;
				srcSubresource.layerCount	  = command.TextureCopy.Extent.Depth;
			}

			VkOffset3D srcOffset;
			srcOffset.x = command.TextureCopy.SourceOffset.X;
			srcOffset.y = command.TextureCopy.SourceOffset.Y;
			srcOffset.z = 0;

			if (srcTexture->GetType() != TextureType::Texture2D)
			{
				srcOffset.z = command.TextureCopy.SourceOffset.Z;
			}

			VkImageSubresourceLayers dstSubresource;
			dstSubresource.aspectMask	  = dstAspect;
			dstSubresource.mipLevel		  = command.TextureCopy.DestinationMipLevel;
			dstSubresource.baseArrayLayer = 0;
			dstSubresource.layerCount	  = 1;

			// we can only set these parameters for array textures, i.e. not 3D textures
			if (dstTexture->GetType() != TextureType::Texture3D)
			{
				dstSubresource.baseArrayLayer = command.TextureCopy.DestinationOffset.Z;
				dstSubresource.layerCount	  = command.TextureCopy.Extent.Depth;
			}

			VkOffset3D dstOffset;
			dstOffset.x = command.TextureCopy.DestinationOffset.X;
			dstOffset.y = command.TextureCopy.DestinationOffset.Y;
			dstOffset.z = 0;

			if (dstTexture->GetType() != TextureType::Texture2D)
			{
				dstOffset.z = command.TextureCopy.DestinationOffset.Z;
			}

			VkExtent3D copyExtent;
			copyExtent.width  = command.TextureCopy.Extent.Width;
			copyExtent.height = command.TextureCopy.Extent.Height;
			copyExtent.depth  = command.TextureCopy.Extent.Depth;

			const GladVulkanContext &context = m_Device->GetVulkanContext();

			if (context.CmdCopyImage2KHR)
			{
				VkImageCopy2KHR copyRegion = {};
				copyRegion.sType		   = VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR;
				copyRegion.pNext		   = nullptr;
				copyRegion.srcSubresource  = srcSubresource;
				copyRegion.srcOffset	   = srcOffset;
				copyRegion.dstSubresource  = dstSubresource;
				copyRegion.dstOffset	   = dstOffset;
				copyRegion.extent		   = copyExtent;

				VkCopyImageInfo2KHR copyInfo = {};
				copyInfo.sType				 = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR;
				copyInfo.pNext				 = nullptr;
				copyInfo.srcImage			 = srcTexture->GetImage();
				copyInfo.srcImageLayout		 = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				copyInfo.dstImage			 = dstTexture->GetImage();
				copyInfo.dstImageLayout		 = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				copyInfo.regionCount		 = 1;
				copyInfo.pRegions			 = &copyRegion;

				context.CmdCopyImage2KHR(m_CommandBuffer, &copyInfo);
			}
			else
			{
				VkImageCopy copyRegion = {};

				// src
				copyRegion.srcSubresource = srcSubresource;
				copyRegion.srcOffset	  = srcOffset;

				// dst
				copyRegion.dstSubresource = dstSubresource;
				copyRegion.dstOffset	  = dstOffset;

				// copy extents
				copyRegion.extent = copyExtent;

				context.CmdCopyImage(m_CommandBuffer,
									 srcTexture->GetImage(),
									 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
									 dstTexture->GetImage(),
									 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									 1,
									 &copyRegion);
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdBeginDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.GroupName.c_str();
			labelEXT.color[0]			  = command.Colour.r;
			labelEXT.color[1]			  = command.Colour.g;
			labelEXT.color[2]			  = command.Colour.b;
			labelEXT.color[3]			  = command.Colour.a;
			context.CmdBeginDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (context.CmdDebugMarkerBeginEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pNext					  = nullptr;
			markerInfo.pMarkerName				  = command.GroupName.c_str();
			markerInfo.color[0]					  = command.Colour.r;
			markerInfo.color[1]					  = command.Colour.g;
			markerInfo.color[2]					  = command.Colour.b;
			markerInfo.color[3]					  = command.Colour.a;
			context.CmdDebugMarkerBeginEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		// if this is the last command in the buffer, then we must explicitly stop rendering to ensure that the implict render pass management
		// occurs in the correct order
		if (m_CurrentCommandIndex >= m_Commands.size() - 1)
		{
			StopRendering();
		}
		// otherwise, if the next command is to set a new render target, we need to stop rendering to ensure that they show in the correct
		// order in debuggers
		else
		{
			RenderCommandData data = m_Commands.at(m_CurrentCommandIndex);
			if (std::holds_alternative<WeakRef<IFramebuffer>>(data))
			{
				StopRendering();
			}
		}

		if (context.CmdEndDebugUtilsLabelEXT)
		{
			context.CmdEndDebugUtilsLabelEXT(m_CommandBuffer);
		}
		else if (context.CmdDebugMarkerEndEXT)
		{
			context.CmdDebugMarkerEndEXT(m_CommandBuffer);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdInsertDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.MarkerName.c_str();
			labelEXT.color[0]			  = command.Colour.r;
			labelEXT.color[1]			  = command.Colour.g;
			labelEXT.color[2]			  = command.Colour.b;
			labelEXT.color[3]			  = command.Colour.a;
			context.CmdInsertDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (context.CmdDebugMarkerInsertEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pNext					  = nullptr;
			markerInfo.pMarkerName				  = command.MarkerName.c_str();
			markerInfo.color[0]					  = command.Colour.r;
			markerInfo.color[1]					  = command.Colour.g;
			markerInfo.color[2]					  = command.Colour.b;
			markerInfo.color[3]					  = command.Colour.a;
			context.CmdDebugMarkerInsertEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
	{
		float blendConstants[4] = {command.BlendFactorDesc.Red,
								   command.BlendFactorDesc.Green,
								   command.BlendFactorDesc.Blue,
								   command.BlendFactorDesc.Alpha};

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetBlendConstants(m_CommandBuffer, blendConstants);
	}

	void CommandExecutorVk::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetStencilReference(m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, command.StencilReference);
	}

	void CommandExecutorVk::ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)
	{
		// return early if the function is not available to use
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		if (!context.CmdBuildAccelerationStructuresKHR)
		{
			return;
		}

		// create storage for the data
		std::vector<std::vector<VkAccelerationStructureGeometryKHR>>	   accelerationStructureGeometries = {};
		std::vector<VkAccelerationStructureBuildGeometryInfoKHR>		   buildGeometries				   = {};
		std::vector<std::vector<VkAccelerationStructureBuildRangeInfoKHR>> buildRanges					   = {};

		// loop through all requested builds
		for (const auto &[buildGeometryInfo, buildRangeInfos] : command.BuildDescriptions)
		{
			// validate that required members have been filled in correctly
			NX_VALIDATE(buildGeometryInfo.Destination, "Acceleration structure build must have a destination");
			NX_VALIDATE(buildGeometryInfo.ScratchBuffer.Buffer, "Acceleration structure build must have a scratch buffer");

			if (buildGeometryInfo.Mode == AccelerationStructureBuildMode::Update)
			{
				NX_VALIDATE(buildGeometryInfo.Source, "Acceleration structure update must have a source");
			}

			// create a new vector to hold the information for the individual build
			std::vector<VkAccelerationStructureGeometryKHR> &accelerationStructureGeometry = accelerationStructureGeometries.emplace_back();

			// create the new build description
			buildGeometries.push_back(Vk::GetGeometryBuildInfo(buildGeometryInfo, accelerationStructureGeometry));

			// create a new vector to hold the build range
			std::vector<VkAccelerationStructureBuildRangeInfoKHR> &geometryBuildRange = buildRanges.emplace_back();

			// iterate through each build range and convert them to Vulkan types
			for (const auto &buildRange : buildRangeInfos) { geometryBuildRange.push_back(Vk::GetAccelerationStructureBuildRange(buildRange)); }
		}

		// execute the acceleration structure build
		context.CmdBuildAccelerationStructuresKHR(m_CommandBuffer,
												  command.BuildDescriptions.size(),
												  buildGeometries.data(),
												  (const VkAccelerationStructureBuildRangeInfoKHR *const *)buildRanges.data());
	}

	void CommandExecutorVk::ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
	{
		if (!m_CurrentlyBoundResourceSet)
			return;

		std::optional<VkShaderStageFlags> stageFlags = m_CurrentlyBoundResourceSet->GetPushConstantsStageFlags(command.Name);

		if (!stageFlags.has_value())
			return;

		if (Ref<PipelineVk> pipeline = std::dynamic_pointer_cast<PipelineVk>(m_CurrentlyBoundPipeline.lock()))
		{
			const GladVulkanContext &context = m_Device->GetVulkanContext();

			if (context.CmdPushConstants2)
			{
				VkPushConstantsInfo pushConstantsInfo = {};
				pushConstantsInfo.sType				  = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
				pushConstantsInfo.pNext				  = nullptr;
				pushConstantsInfo.layout			  = pipeline->GetPipelineLayout();
				pushConstantsInfo.stageFlags		  = stageFlags.value();
				pushConstantsInfo.offset			  = command.Offset;
				pushConstantsInfo.size				  = command.Data.size();
				pushConstantsInfo.pValues			  = command.Data.data();

				context.CmdPushConstants2(m_CommandBuffer, &pushConstantsInfo);
			}
			else if (context.CmdPushConstants2KHR)
			{
				VkPushConstantsInfoKHR pushConstantsInfo = {};
				pushConstantsInfo.sType					 = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO_KHR;
				pushConstantsInfo.pNext					 = nullptr;
				pushConstantsInfo.layout				 = pipeline->GetPipelineLayout();
				pushConstantsInfo.stageFlags			 = stageFlags.value();
				pushConstantsInfo.offset				 = command.Offset;
				pushConstantsInfo.size					 = command.Data.size();
				pushConstantsInfo.pValues				 = command.Data.data();

				context.CmdPushConstants2KHR(m_CommandBuffer, &pushConstantsInfo);
			}
			else
			{
				context.CmdPushConstants(m_CommandBuffer,
										 pipeline->GetPipelineLayout(),
										 stageFlags.value(),
										 command.Offset,
										 command.Data.size(),
										 command.Data.data());
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const MemoryBarrierDesc &command, IGraphicsDevice *device)
	{
		if (m_Rendering)
		{
			StopRendering();
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		// for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to be exposed in future
		VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if (context.CmdPipelineBarrier2KHR)
		{
			VkAccessFlagBits2 srcAccess = Vk::GetAccessFlags2(m_Device, command.BeforeAccess);
			VkAccessFlagBits2 dstAccess = Vk::GetAccessFlags2(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits2 srcStage = Vk::GetPipelineStageFlags2(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits2 dstStage = Vk::GetPipelineStageFlags2(m_Device, command.AfterStage);

			VkMemoryBarrier2KHR memoryBarrier = {};
			memoryBarrier.sType				  = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
			memoryBarrier.pNext				  = nullptr;
			memoryBarrier.srcStageMask		  = srcStage;
			memoryBarrier.dstStageMask		  = dstStage;
			memoryBarrier.srcAccessMask		  = srcAccess;
			memoryBarrier.dstAccessMask		  = dstAccess;

			VkDependencyInfoKHR dependencyInfo		= {};
			dependencyInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
			dependencyInfo.pNext					= nullptr;
			dependencyInfo.dependencyFlags			= dependencyFlags;
			dependencyInfo.memoryBarrierCount		= 1;
			dependencyInfo.pMemoryBarriers			= &memoryBarrier;
			dependencyInfo.bufferMemoryBarrierCount = 0;
			dependencyInfo.pBufferMemoryBarriers	= nullptr;
			dependencyInfo.imageMemoryBarrierCount	= 0;
			dependencyInfo.pImageMemoryBarriers		= nullptr;

			context.CmdPipelineBarrier2KHR(m_CommandBuffer, &dependencyInfo);
		}
		else
		{
			VkAccessFlagBits srcAccess = Vk::GetAccessFlags(m_Device, command.BeforeAccess);
			VkAccessFlagBits dstAccess = Vk::GetAccessFlags(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits srcStage = Vk::GetPipelineStageFlags(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits dstStage = Vk::GetPipelineStageFlags(m_Device, command.AfterStage);

			VkMemoryBarrier barrier = {};
			barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			barrier.pNext			= nullptr;
			barrier.srcAccessMask	= srcAccess;
			barrier.dstAccessMask	= dstAccess;

			context.CmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, dependencyFlags, 1, &barrier, 0, nullptr, 0, nullptr);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const TextureBarrierDesc &command, IGraphicsDevice *device)
	{
		if (m_Rendering)
		{
			StopRendering();
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		Ref<TextureVk> texture	 = std::dynamic_pointer_cast<TextureVk>(command.ITexture);
		VkImageLayout  newLayout = Vk::GetImageLayout(m_Device, command.Layout);

		// for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to be exposed in future
		VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		struct SubresourceRangeLayout
		{
			VkImageSubresourceRange range;
			VkImageLayout			layout;
		};

		std::vector<SubresourceRangeLayout> ranges;

		for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
			 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
				 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
				 mipLevel++)
			{
				SubresourceRangeLayout &range = ranges.emplace_back();
				range.range.aspectMask		  = Vk::GetAspectFlags(texture->IsDepth());
				range.range.baseArrayLayer	  = arrayLayer;
				range.range.layerCount		  = 1;
				range.range.baseMipLevel	  = mipLevel;
				range.range.levelCount		  = 1;
				range.layout				  = Vk::GetImageLayout(m_Device, texture->GetTextureLayout(arrayLayer, mipLevel));
			}
		}

		if (context.CmdPipelineBarrier2KHR)
		{
			VkAccessFlagBits2 srcAccess = Vk::GetAccessFlags2(m_Device, command.BeforeAccess);
			VkAccessFlagBits2 dstAccess = Vk::GetAccessFlags2(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits2 srcStage = Vk::GetPipelineStageFlags2(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits2 dstStage = Vk::GetPipelineStageFlags2(m_Device, command.AfterStage);

			std::vector<VkImageMemoryBarrier2KHR> imageBarriers = {};

			for (const auto &[subresourceRange, imageLayout] : ranges)
			{
				VkImageMemoryBarrier2KHR &imageBarrier = imageBarriers.emplace_back();
				imageBarrier.sType					   = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
				imageBarrier.pNext					   = nullptr;
				imageBarrier.srcStageMask			   = srcStage;
				imageBarrier.dstStageMask			   = dstStage;
				imageBarrier.srcAccessMask			   = srcAccess;
				imageBarrier.dstAccessMask			   = dstAccess;
				imageBarrier.oldLayout				   = imageLayout;
				imageBarrier.newLayout				   = newLayout;
				imageBarrier.srcQueueFamilyIndex	   = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex	   = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.image					   = texture->GetImage();
				imageBarrier.subresourceRange		   = subresourceRange;
			}

			VkDependencyInfoKHR dependencyInfo		= {};
			dependencyInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
			dependencyInfo.pNext					= nullptr;
			dependencyInfo.dependencyFlags			= dependencyFlags;
			dependencyInfo.memoryBarrierCount		= 0;
			dependencyInfo.pMemoryBarriers			= nullptr;
			dependencyInfo.bufferMemoryBarrierCount = 0;
			dependencyInfo.pBufferMemoryBarriers	= nullptr;
			dependencyInfo.imageMemoryBarrierCount	= imageBarriers.size();
			dependencyInfo.pImageMemoryBarriers		= imageBarriers.data();

			context.CmdPipelineBarrier2KHR(m_CommandBuffer, &dependencyInfo);
		}
		else
		{
			VkAccessFlagBits srcAccess = Vk::GetAccessFlags(m_Device, command.BeforeAccess);
			VkAccessFlagBits dstAccess = Vk::GetAccessFlags(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits srcStage = Vk::GetPipelineStageFlags(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits dstStage = Vk::GetPipelineStageFlags(m_Device, command.AfterStage);

			std::vector<VkImageMemoryBarrier> imageBarriers = {};
			for (const auto &[subresourceRange, imageLayout] : ranges)
			{
				VkImageMemoryBarrier &imageBarrier = imageBarriers.emplace_back();
				imageBarrier.sType				   = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrier.pNext				   = nullptr;
				imageBarrier.srcAccessMask		   = srcAccess;
				imageBarrier.dstAccessMask		   = dstAccess;
				imageBarrier.oldLayout			   = imageLayout;
				imageBarrier.newLayout			   = newLayout;
				imageBarrier.srcQueueFamilyIndex   = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex   = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.image				   = texture->GetImage();
				imageBarrier.subresourceRange	   = subresourceRange;
			}

			context.CmdPipelineBarrier(m_CommandBuffer,
									   srcStage,
									   dstStage,
									   dependencyFlags,
									   0,
									   nullptr,
									   0,
									   nullptr,
									   imageBarriers.size(),
									   imageBarriers.data());
		}

		for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
			 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
				 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
				 mipLevel++)
			{
				texture->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
			}
		}
	}

	void CommandExecutorVk::ExecuteCommand(const BufferBarrierDesc &command, IGraphicsDevice *device)
	{
		if (m_Rendering)
		{
			StopRendering();
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		Ref<DeviceBufferVk> bufferVk = std::dynamic_pointer_cast<DeviceBufferVk>(command.Buffer);

		// for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to be exposed in future
		VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if (context.CmdPipelineBarrier2KHR)
		{
			VkAccessFlagBits2 srcAccess = Vk::GetAccessFlags2(m_Device, command.BeforeAccess);
			VkAccessFlagBits2 dstAccess = Vk::GetAccessFlags2(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits2 srcStage = Vk::GetPipelineStageFlags2(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits2 dstStage = Vk::GetPipelineStageFlags2(m_Device, command.AfterStage);

			VkBufferMemoryBarrier2KHR bufferBarrier = {};
			bufferBarrier.sType						= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
			bufferBarrier.pNext						= nullptr;
			bufferBarrier.srcStageMask				= srcStage;
			bufferBarrier.dstStageMask				= dstStage;
			bufferBarrier.srcAccessMask				= srcAccess;
			bufferBarrier.dstAccessMask				= dstAccess;
			bufferBarrier.srcQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.dstQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.buffer					= bufferVk->GetVkBuffer();
			bufferBarrier.offset					= command.Offset;
			bufferBarrier.size						= command.Size;

			VkDependencyInfoKHR dependencyInfo		= {};
			dependencyInfo.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
			dependencyInfo.pNext					= nullptr;
			dependencyInfo.dependencyFlags			= dependencyFlags;
			dependencyInfo.memoryBarrierCount		= 0;
			dependencyInfo.pMemoryBarriers			= nullptr;
			dependencyInfo.bufferMemoryBarrierCount = 1;
			dependencyInfo.pBufferMemoryBarriers	= &bufferBarrier;
			dependencyInfo.imageMemoryBarrierCount	= 0;
			dependencyInfo.pImageMemoryBarriers		= nullptr;

			context.CmdPipelineBarrier2KHR(m_CommandBuffer, &dependencyInfo);
		}
		else
		{
			VkAccessFlagBits srcAccess = Vk::GetAccessFlags(m_Device, command.BeforeAccess);
			VkAccessFlagBits dstAccess = Vk::GetAccessFlags(m_Device, command.AfterAccess);

			VkPipelineStageFlagBits srcStage = Vk::GetPipelineStageFlags(m_Device, command.BeforeStage);
			VkPipelineStageFlagBits dstStage = Vk::GetPipelineStageFlags(m_Device, command.AfterStage);

			VkBufferMemoryBarrier barrier = {};
			barrier.sType				  = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barrier.pNext				  = nullptr;
			barrier.srcAccessMask		  = srcAccess;
			barrier.dstAccessMask		  = dstAccess;
			barrier.srcQueueFamilyIndex	  = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex	  = VK_QUEUE_FAMILY_IGNORED;
			barrier.buffer				  = bufferVk->GetVkBuffer();
			barrier.offset				  = command.Offset;
			barrier.size				  = command.Size;

			context.CmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, dependencyFlags, 0, nullptr, 1, &barrier, 0, nullptr);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
	{
	}

	void BeginRenderPass(GraphicsDeviceVk			 *device,
						 const VkRenderPassBeginInfo &beginInfo,
						 VkSubpassContents			  subpassContents,
						 VkCommandBuffer			  commandBuffer)
	{
		const GladVulkanContext &context = device->GetVulkanContext();

		if (context.CmdBeginRenderPass2KHR)
		{
			VkSubpassBeginInfo subpassInfo = {};
			subpassInfo.sType			   = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
			subpassInfo.pNext			   = nullptr;
			subpassInfo.contents		   = subpassContents;

			context.CmdBeginRenderPass2KHR(commandBuffer, &beginInfo, &subpassInfo);
		}
		else
		{
			context.CmdBeginRenderPass(commandBuffer, &beginInfo, subpassContents);
		}
	}

	void BeginDynamicRenderingToFramebuffer(GraphicsDeviceVk *device, Ref<FramebufferVk> framebuffer, VkCommandBuffer commandBuffer)
	{
		const GladVulkanContext &context = device->GetVulkanContext();

		VkRect2D renderArea {};
		renderArea.offset = {0, 0};
		renderArea.extent = {framebuffer->GetWidth(), framebuffer->GetHeight()};

		std::vector<VkRenderingAttachmentInfo> colourAttachments;

		// attach colour textures
		for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < framebuffer->GetColorTextureCount(); colourAttachmentIndex++)
		{
			FramebufferColourAttachmentDescription textureBinding = framebuffer->GetColorTextureBinding(colourAttachmentIndex).value();

			Ref<TextureVk> texture = std::dynamic_pointer_cast<TextureVk>(textureBinding.ColourAttachment.TargetTexture);
			TextureLayout  layout =
				texture->GetTextureLayout(textureBinding.ColourAttachment.BaseArrayLayer, textureBinding.ColourAttachment.MipLevel);

			FramebufferColourAttachmentDescription colourAttachmentDesc = framebuffer->GetColorTextureBinding(colourAttachmentIndex).value();

			VulkanTextureViewInfo viewInfo = {};
			viewInfo.BaseMipLevel		   = colourAttachmentDesc.ColourAttachment.MipLevel;
			viewInfo.LevelCount			   = 1;
			viewInfo.BaseArrayLayer		   = colourAttachmentDesc.ColourAttachment.BaseArrayLayer;
			viewInfo.LayerCount			   = colourAttachmentDesc.ColourAttachment.LayerCount;

			VkRenderingAttachmentInfo colourAttachment = {};
			colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colourAttachment.imageView				   = texture->GetImageView(viewInfo);
			colourAttachment.imageLayout			   = Vk::GetImageLayout(device, layout);
			colourAttachment.loadOp					   = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp				   = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.clearValue				   = {};

			// handle the case of submitting a swapchain with multisampling
			if (colourAttachmentDesc.ResolveAttachment.has_value())
			{
				FramebufferTextureDescription resolveDesc		= colourAttachmentDesc.ResolveAttachment.value();
				Ref<TextureVk>				  resolveAttachment = std::dynamic_pointer_cast<TextureVk>(resolveDesc.TargetTexture);
				TextureLayout				  resolveLayout = resolveAttachment->GetTextureLayout(resolveDesc.BaseArrayLayer, resolveDesc.MipLevel);

				VulkanTextureViewInfo viewInfo = {};
				viewInfo.BaseMipLevel		   = resolveDesc.MipLevel;
				viewInfo.LevelCount			   = 1;
				viewInfo.BaseArrayLayer		   = resolveDesc.BaseArrayLayer;
				viewInfo.LayerCount			   = resolveDesc.LayerCount;

				colourAttachment.resolveImageView	= resolveAttachment->GetImageView(viewInfo);
				colourAttachment.resolveImageLayout = Vk::GetImageLayout(device, resolveLayout);
				colourAttachment.resolveMode		= VK_RESOLVE_MODE_AVERAGE_BIT;
			}

			colourAttachments.push_back(colourAttachment);
		}

		// set up depth attachment (may be unused)
		VkRenderingAttachmentInfo depthAttachment = {};
		if (framebuffer->HasDepthTexture())
		{
			FramebufferTextureDescription textureBinding = framebuffer->GetDepthTextureBinding().value();

			Ref<TextureVk> texture = framebuffer->GetVulkanDepthTexture();
			TextureLayout  layout  = texture->GetTextureLayout(textureBinding.BaseArrayLayer, textureBinding.MipLevel);

			FramebufferTextureDescription depthAttachmentDesc = framebuffer->GetDepthTextureBinding().value();

			VulkanTextureViewInfo viewInfo = {};
			viewInfo.BaseMipLevel		   = depthAttachmentDesc.MipLevel;
			viewInfo.LevelCount			   = 1;
			viewInfo.BaseArrayLayer		   = depthAttachmentDesc.BaseArrayLayer;
			viewInfo.LayerCount			   = depthAttachmentDesc.LayerCount;

			depthAttachment.sType		= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView	= texture->GetImageView(viewInfo);
			depthAttachment.imageLayout = Vk::GetImageLayout(device, layout);
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

		if (framebuffer->HasDepthTexture())
		{
			renderingInfo.pDepthAttachment	 = &depthAttachment;
			renderingInfo.pStencilAttachment = &depthAttachment;
		}
		else
		{
			renderingInfo.pDepthAttachment	 = nullptr;
			renderingInfo.pStencilAttachment = nullptr;
		}

		context.CmdBeginRenderingKHR(commandBuffer, &renderingInfo);
	}

	void BeginRenderPassToFramebuffer(GraphicsDeviceVk *device, Ref<FramebufferVk> framebuffer, VkCommandBuffer commandBuffer)
	{
		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext					= nullptr;
		beginInfo.renderPass			= framebuffer->GetRenderPass();
		beginInfo.framebuffer			= framebuffer->GetFramebuffer();
		beginInfo.renderArea.offset		= {0, 0};
		beginInfo.renderArea.extent		= {framebuffer->GetWidth(), framebuffer->GetHeight()};
		beginInfo.clearValueCount		= 0;
		beginInfo.pClearValues			= nullptr;

		VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE;

		BeginRenderPass(device, beginInfo, subpassContents, commandBuffer);
	}

	void CommandExecutorVk::StartRenderingToFramebuffer(Ref<IFramebuffer> framebuffer)
	{
		Ref<FramebufferVk> vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

		const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
		if (features.DynamicRenderingAvailable)
		{
			BeginDynamicRenderingToFramebuffer(m_Device, vulkanFramebuffer, m_CommandBuffer);
		}
		else
		{
			BeginRenderPassToFramebuffer(m_Device, vulkanFramebuffer, m_CommandBuffer);
		}

		m_Rendering = true;
	}

	void CommandExecutorVk::StopRendering()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (m_Rendering)
		{
			const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
			if (features.DynamicRenderingAvailable)
			{
				const GladVulkanContext &context = m_Device->GetVulkanContext();
				context.CmdEndRenderingKHR(m_CommandBuffer);
			}
			else
			{
				context.CmdEndRenderPass(m_CommandBuffer);
			}
		}

		m_Rendering = false;
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

	void CommandExecutorVk::BindGraphicsPipeline()
	{
		auto						vulkanPipeline = std::dynamic_pointer_cast<PipelineVk>(m_CurrentlyBoundPipeline.lock());
		const VulkanDeviceFeatures &deviceFeatures = m_Device->GetDeviceFeatures();

		if (m_CurrentRenderTarget)
		{
			VkRenderPass renderPass = VK_NULL_HANDLE;

			const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
			if (!features.DynamicRenderingAvailable)
			{
				Ref<FramebufferVk> framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(m_CurrentRenderTarget);
				renderPass						 = framebufferVk->GetRenderPass();
				vulkanPipeline->Bind(m_CommandBuffer, renderPass);
			}
		}
		else
		{
			throw std::runtime_error("Failed to find a valid render target type");
		}
	}

	void CommandExecutorVk::TryStartRendering()
	{
		if (!m_Rendering)
		{
			if (m_CurrentRenderTarget)
			{
				StartRenderingToFramebuffer(m_CurrentRenderTarget);
			}
		}
	}
}	 // namespace Nexus::Graphics

#endif