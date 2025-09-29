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

	void CommandExecutorVk::ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(const SetVertexBufferCommand &command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(const SetIndexBufferCommand &command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(const DrawDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDraw(m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDrawIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDrawIndexedIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdDispatch(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		if (Ref<DeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferVk>		 indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(buffer);
			const GladVulkanContext &context		= m_Device->GetVulkanContext();
			context.CmdDispatchIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(const DrawMeshIndirectDescription &command, GraphicsDevice *device)
	{
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

	void CommandExecutorVk::ExecuteCommand(const ClearColorTargetCommand &command, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(const ClearDepthStencilTargetCommand &command, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		StopRendering();

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

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetViewport(m_CommandBuffer, 0, 1, &vp);
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

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetScissor(m_CommandBuffer, 0, 1, &rect);
	}

	void CommandExecutorVk::ExecuteCommand(const ResolveSamplesToSwapchainCommand &command, GraphicsDevice *device)
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

		VkImageLayout framebufferLayout =
			Vk::GetImageLayout(m_Device, framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetTextureLayout(0, 0));
		VkImageLayout swapchainLayout = swapchainVk->GetColorImageLayout();

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdResolveImage(m_CommandBuffer, framebufferImage, framebufferLayout, swapchainImage, swapchainLayout, 1, &resolve);

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk>		 queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		context.CmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
		context.CmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
	}

	void CommandExecutorVk::ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk>		 queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		context.CmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
	}

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
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
			imageSubresource.mipLevel				  = command.BufferTextureCopy.TextureSubresource.MipLevel;
			imageSubresource.baseArrayLayer			  = command.BufferTextureCopy.TextureSubresource.BaseArrayLayer;
			imageSubresource.layerCount				  = command.BufferTextureCopy.TextureSubresource.LayerCount;

			VkOffset3D imageOffset = {};
			imageOffset.x		   = command.BufferTextureCopy.TextureOffset.X;
			imageOffset.y		   = command.BufferTextureCopy.TextureOffset.Y;
			imageOffset.z		   = 0;

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

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
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
			imageSubresource.mipLevel				  = command.TextureBufferCopy.TextureSubresource.MipLevel;
			imageSubresource.baseArrayLayer			  = command.TextureBufferCopy.TextureSubresource.BaseArrayLayer;
			imageSubresource.layerCount				  = command.TextureBufferCopy.TextureSubresource.LayerCount;

			VkOffset3D imageOffset = {};
			imageOffset.x		   = command.TextureBufferCopy.TextureOffset.X;
			imageOffset.y		   = command.TextureBufferCopy.TextureOffset.Y;
			imageOffset.z		   = command.TextureBufferCopy.TextureOffset.Z;

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

	void CommandExecutorVk::ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)
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
			srcSubresource.mipLevel		  = command.TextureCopy.SourceSubresource.MipLevel;
			srcSubresource.baseArrayLayer = command.TextureCopy.SourceSubresource.BaseArrayLayer;
			srcSubresource.layerCount	  = command.TextureCopy.SourceSubresource.LayerCount;

			VkOffset3D srcOffset;
			srcOffset.x = command.TextureCopy.SourceOffset.X;
			srcOffset.y = command.TextureCopy.SourceOffset.Y;
			srcOffset.z = command.TextureCopy.SourceOffset.Z;

			VkImageSubresourceLayers dstSubresource;
			dstSubresource.aspectMask	  = dstAspect;
			dstSubresource.mipLevel		  = command.TextureCopy.DestinationSubresource.MipLevel;
			dstSubresource.baseArrayLayer = command.TextureCopy.DestinationSubresource.BaseArrayLayer;
			dstSubresource.layerCount	  = command.TextureCopy.DestinationSubresource.LayerCount;

			VkOffset3D dstOffset;
			dstOffset.x = command.TextureCopy.DestinationOffset.X;
			dstOffset.y = command.TextureCopy.DestinationOffset.Y;
			dstOffset.z = command.TextureCopy.DestinationOffset.Z;

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

	void CommandExecutorVk::ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdBeginDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.GroupName.c_str();
			labelEXT.color[0]			  = 0;
			labelEXT.color[1]			  = 0;
			labelEXT.color[2]			  = 0;
			labelEXT.color[3]			  = 0;
			context.CmdBeginDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (context.CmdDebugMarkerBeginEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pMarkerName				  = command.GroupName.c_str();
			context.CmdDebugMarkerBeginEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device)
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
			if (std::holds_alternative<RenderTarget>(data))
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

	void CommandExecutorVk::ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		if (context.CmdInsertDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.MarkerName.c_str();
			labelEXT.color[0]			  = 0;
			labelEXT.color[1]			  = 0;
			labelEXT.color[2]			  = 0;
			labelEXT.color[3]			  = 0;
			context.CmdInsertDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (context.CmdDebugMarkerInsertEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pMarkerName				  = command.MarkerName.c_str();
			context.CmdDebugMarkerInsertEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device)
	{
		float blendConstants[4] = {command.BlendFactorDesc.Red,
								   command.BlendFactorDesc.Green,
								   command.BlendFactorDesc.Blue,
								   command.BlendFactorDesc.Alpha};

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetBlendConstants(m_CommandBuffer, blendConstants);
	}

	void CommandExecutorVk::ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device)
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.CmdSetStencilReference(m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, command.StencilReference);
	}

	void CommandExecutorVk::ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(const AccelerationStructureCopyDescription &command, GraphicsDevice *Device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorVk::ExecuteCommand(const MemoryBarrierDesc &command, GraphicsDevice *device)
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

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(const TextureBarrierDesc &command, GraphicsDevice *device)
	{
		if (m_Rendering)
		{
			StopRendering();
		}

		const GladVulkanContext &context = m_Device->GetVulkanContext();

		Ref<TextureVk> texture	 = std::dynamic_pointer_cast<TextureVk>(command.Texture);
		VkImageLayout  newLayout = Vk::GetImageLayout(m_Device, command.Layout);

		// for now VK_DEPENDENCY_BY_REGION_BIT is hardcoded, however this may need to be exposed in future
		VkDependencyFlagBits dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		struct SubresourceRangeLayout
		{
			VkImageSubresourceRange range;
			VkImageLayout			layout;
		};

		std::vector<SubresourceRangeLayout> ranges;

		VkImageLayout currentLayout =
			Vk::GetImageLayout(m_Device, texture->GetTextureLayout(command.SubresourceRange.BaseArrayLayer, command.SubresourceRange.BaseMipLevel));
		uint32_t arrayLayerStart = command.SubresourceRange.BaseArrayLayer;
		uint32_t mipLevelStart	 = command.SubresourceRange.BaseMipLevel;
		uint32_t arrayLayerCount = 0;
		uint32_t mipLevelCount	 = 0;

		for (uint32_t arrayLayer = command.SubresourceRange.BaseArrayLayer;
			 arrayLayer < command.SubresourceRange.BaseArrayLayer + command.SubresourceRange.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mipLevel = command.SubresourceRange.BaseMipLevel;
				 mipLevel < command.SubresourceRange.BaseMipLevel + command.SubresourceRange.LevelCount;
				 mipLevel++)
			{
				VkImageLayout layout = Vk::GetImageLayout(m_Device, texture->GetTextureLayout(arrayLayer, mipLevel));

				// start new subresource range
				if (arrayLayerCount == 0)
				{
					arrayLayerStart = arrayLayer;
					mipLevelStart	= mipLevel;
					arrayLayerCount = 1;
					mipLevelCount	= 1;
					currentLayout	= layout;
				}
				// if the layout is the same and we are still in the same array layer, then extend the range
				else if (layout == currentLayout && arrayLayer == arrayLayerStart && mipLevel == mipLevelStart + mipLevelCount)
				{
					++mipLevelCount;
				}
				// otherwise, we need to record this range and begin a new one
				else
				{
					SubresourceRangeLayout &range = ranges.emplace_back();
					range.range.aspectMask		  = Vk::GetAspectFlags(texture->IsDepth());
					range.range.baseArrayLayer	  = arrayLayerStart;
					range.range.layerCount		  = arrayLayerCount;
					range.range.baseMipLevel	  = mipLevelStart;
					range.range.levelCount		  = mipLevelCount;
					range.layout				  = currentLayout;
					currentLayout				  = layout;
				}
			}
		}

		// flush the final range
		if (arrayLayerCount > 0)
		{
			SubresourceRangeLayout &range = ranges.emplace_back();
			range.range.aspectMask		  = Vk::GetAspectFlags(texture->IsDepth());
			range.range.baseArrayLayer	  = arrayLayerStart;
			range.range.layerCount		  = arrayLayerCount;
			range.range.baseMipLevel	  = mipLevelStart;
			range.range.levelCount		  = mipLevelCount;
			range.layout				  = currentLayout;
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

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(const BufferBarrierDesc &command, GraphicsDevice *device)
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

		ExecuteCommand(m_CurrentRenderTarget, device);
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

	void BeginDynamicRenderingToSwapchain(GraphicsDeviceVk *device, Ref<SwapchainVk> swapchain, VkCommandBuffer commandBuffer)
	{
		VkExtent2D swapchainSize = swapchain->GetSwapchainSize();

		VkRect2D renderArea;
		renderArea.offset = {0, 0};
		renderArea.extent = swapchainSize;

		VkRenderingAttachmentInfo colourAttachment = {};
		colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;

		if (swapchain->GetDescription().Samples == 1)
		{
			colourAttachment.imageView	 = swapchain->GetColourImageView();
			colourAttachment.imageLayout = swapchain->GetColorImageLayout();
		}
		else
		{
			swapchain->SetResolveImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			colourAttachment.imageView	 = swapchain->GetResolveImageView();
			colourAttachment.imageLayout = swapchain->GetResolveImageLayout();

			colourAttachment.resolveImageView	= swapchain->GetColourImageView();
			colourAttachment.resolveImageLayout = swapchain->GetColorImageLayout();
			colourAttachment.resolveMode		= VK_RESOLVE_MODE_AVERAGE_BIT;
		}

		colourAttachment.loadOp		= VK_ATTACHMENT_LOAD_OP_LOAD;
		colourAttachment.storeOp	= VK_ATTACHMENT_STORE_OP_STORE;
		colourAttachment.clearValue = {};

		VkRenderingAttachmentInfo depthAttachment = {};
		depthAttachment.sType					  = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView				  = swapchain->GetDepthImageView();
		depthAttachment.imageLayout				  = swapchain->GetDepthImageLayout();
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

		const GladVulkanContext &context = device->GetVulkanContext();
		context.CmdBeginRenderingKHR(commandBuffer, &renderingInfo);
	}

	void BeginRenderPassToSwapchain(GraphicsDeviceVk *device, Ref<SwapchainVk> swapchain, VkCommandBuffer commandBuffer)
	{
		VkFramebuffer framebuffer = swapchain->GetFramebuffer();
		VkRenderPass  renderpass  = swapchain->GetRenderPass();
		VkExtent2D	  renderSize  = swapchain->GetSwapchainSize();

		NX_VALIDATE(framebuffer, "Invalid framebuffer");
		NX_VALIDATE(renderpass, "Invalid renderpass");
		NX_VALIDATE(renderSize.width > 0 && renderSize.height > 0, "Invalid render size");

		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext					= nullptr;
		beginInfo.renderPass			= renderpass;
		beginInfo.framebuffer			= framebuffer;
		beginInfo.renderArea.offset		= {0, 0};
		beginInfo.renderArea.extent		= renderSize;
		beginInfo.clearValueCount		= 0;
		beginInfo.pClearValues			= nullptr;

		VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE;

		BeginRenderPass(device, beginInfo, subpassContents, commandBuffer);
	}

	void CommandExecutorVk::StartRenderingToSwapchain(Ref<Swapchain> swapchain)
	{
		Ref<SwapchainVk> swapchainVk   = std::dynamic_pointer_cast<SwapchainVk>(swapchain);
		const auto		&swapchainDesc = swapchainVk->GetDescription();

		swapchainVk->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		swapchainVk->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		if (swapchainDesc.Samples != 1)
		{
			/*m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
												  swapchainVk->GetResolveImage(),
												  0,
												  0,
												  swapchainVk->GetResolveImageLayout(),
												  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
												  VK_IMAGE_ASPECT_COLOR_BIT);
			swapchainVk->SetResolveImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);*/
		}

		const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
		if (features.DynamicRenderingAvailable)
		{
			BeginDynamicRenderingToSwapchain(m_Device, swapchainVk, m_CommandBuffer);
		}
		else
		{
			BeginRenderPassToSwapchain(m_Device, swapchainVk, m_CommandBuffer);
		}

		m_Rendering = true;
	}

	void BeginDynamicRenderingToFramebuffer(GraphicsDeviceVk *device, Ref<FramebufferVk> framebuffer, VkCommandBuffer commandBuffer)
	{
		const GladVulkanContext &context = device->GetVulkanContext();

		VkRect2D renderArea {};
		renderArea.offset = {0, 0};
		renderArea.extent = {framebuffer->GetFramebufferSpecification().Width, framebuffer->GetFramebufferSpecification().Height};

		std::vector<VkRenderingAttachmentInfo> colourAttachments;

		// attach colour textures
		for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < framebuffer->GetColorTextureCount(); colourAttachmentIndex++)
		{
			Ref<TextureVk> texture = framebuffer->GetVulkanColorTexture(colourAttachmentIndex);
			TextureLayout  layout  = texture->GetTextureLayout(0, 0);

			VkRenderingAttachmentInfo colourAttachment = {};
			colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colourAttachment.imageView				   = texture->GetImageView();
			colourAttachment.imageLayout			   = Vk::GetImageLayout(device, layout);
			colourAttachment.loadOp					   = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp				   = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.clearValue				   = {};
			colourAttachments.push_back(colourAttachment);
		}

		// set up depth attachment (may be unused)
		VkRenderingAttachmentInfo depthAttachment = {};
		if (framebuffer->HasDepthTexture())
		{
			Ref<TextureVk> texture = framebuffer->GetVulkanDepthTexture();
			TextureLayout  layout  = texture->GetTextureLayout(0, 0);

			depthAttachment.sType		= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView	= texture->GetImageView();
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
		beginInfo.renderArea.extent		= {framebuffer->GetFramebufferSpecification().Width, framebuffer->GetFramebufferSpecification().Height};
		beginInfo.clearValueCount		= 0;
		beginInfo.pClearValues			= nullptr;

		VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE;

		BeginRenderPass(device, beginInfo, subpassContents, commandBuffer);
	}

	void CommandExecutorVk::StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer)
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

		if (Ref<Swapchain> swapchain = m_CurrentRenderTarget.GetSwapchain().lock())
		{
			Ref<SwapchainVk> swapchainVk = std::dynamic_pointer_cast<SwapchainVk>(swapchain);

			VkRenderPass renderPass = VK_NULL_HANDLE;

			const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
			if (!features.DynamicRenderingAvailable)
			{
				renderPass = swapchainVk->GetRenderPass();
				vulkanPipeline->Bind(m_CommandBuffer, renderPass);
			}
		}
		else if (Ref<Framebuffer> framebuffer = m_CurrentRenderTarget.GetFramebuffer().lock())
		{
			VkRenderPass renderPass = VK_NULL_HANDLE;

			const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
			if (!features.DynamicRenderingAvailable)
			{
				Ref<FramebufferVk> framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);
				renderPass						 = framebufferVk->GetRenderPass();
				vulkanPipeline->Bind(m_CommandBuffer, renderPass);
			}
		}
		else
		{
			throw std::runtime_error("Failed to find a valid render target type");
		}
	}
}	 // namespace Nexus::Graphics

#endif