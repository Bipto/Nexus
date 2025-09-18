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

		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		if (functions.vkCmdBindVertexBuffers2EXT)
		{
			functions.vkCmdBindVertexBuffers2EXT(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets, sizes, nullptr);
		}
		else
		{
			vkCmdBindVertexBuffers(m_CommandBuffer, command.Slot, 1, vertexBuffers, offsets);
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

		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		if (functions.vkCmdBindIndexBuffer2KHR)
		{
			functions.vkCmdBindIndexBuffer2KHR(m_CommandBuffer, indexBufferHandle, offset, size, indexType);
		}
		else
		{
			vkCmdBindIndexBuffer(m_CommandBuffer, indexBufferHandle, offset, indexType);
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

		vkCmdDraw(m_CommandBuffer, command.VertexCount, command.InstanceCount, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		vkCmdDrawIndexed(m_CommandBuffer, command.IndexCount, command.InstanceCount, command.IndexStart, command.VertexStart, command.InstanceStart);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		vkCmdDrawIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		Ref<DeviceBufferVk> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferVk>(command.IndirectBuffer);

		BindGraphicsPipeline();

		vkCmdDrawIndexedIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset, command.DrawCount, command.Stride);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		vkCmdDispatch(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
	}

	void CommandExecutorVk::ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device)
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

	void CommandExecutorVk::ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget) || !ValidateIsRendering())
		{
			return;
		}

		BindGraphicsPipeline();

		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
		if (functions.vkCmdDrawMeshTasksEXT)
		{
			functions.vkCmdDrawMeshTasksEXT(m_CommandBuffer, command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
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

		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
		if (functions.vkCmdDrawMeshTasksIndirectEXT)
		{
			functions.vkCmdDrawMeshTasksIndirectEXT(m_CommandBuffer,
													indirectBuffer->GetVkBuffer(),
													command.Offset,
													command.DrawCount,
													command.Stride);
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

		vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
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

		vkCmdClearAttachments(m_CommandBuffer, 1, &clearAttachment, 1, &clearRect);
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

		auto framebufferLayout = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImageLayout(0, 0);
		auto swapchainLayout   = swapchainVk->GetColorImageLayout();

		vkCmdResolveImage(m_CommandBuffer, framebufferImage, framebufferLayout, swapchainImage, swapchainLayout, 1, &resolve);

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		vkCmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
		vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
	}

	void CommandExecutorVk::ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query);
		vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryVk->GetQueryPool(), 1);
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
			VkBufferImageCopy copyRegion			   = {};
			copyRegion.bufferOffset					   = command.BufferTextureCopy.BufferOffset;
			copyRegion.bufferRowLength				   = 0;
			copyRegion.bufferImageHeight			   = 0;
			copyRegion.imageSubresource.aspectMask	   = aspectFlags;
			copyRegion.imageSubresource.mipLevel	   = command.BufferTextureCopy.TextureSubresource.MipLevel;
			copyRegion.imageSubresource.layerCount	   = command.BufferTextureCopy.TextureSubresource.Depth;
			copyRegion.imageOffset.x				   = command.BufferTextureCopy.TextureSubresource.X;
			copyRegion.imageOffset.y				   = command.BufferTextureCopy.TextureSubresource.Y;
			copyRegion.imageOffset.z				   = 0;
			copyRegion.imageExtent.width			   = command.BufferTextureCopy.TextureSubresource.Width;
			copyRegion.imageExtent.height			   = command.BufferTextureCopy.TextureSubresource.Height;
			copyRegion.imageExtent.depth			   = command.BufferTextureCopy.TextureSubresource.Depth;
			copyRegion.imageSubresource.baseArrayLayer = command.BufferTextureCopy.TextureSubresource.ArrayLayer;

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
			VkBufferImageCopy copyRegion			   = {};
			copyRegion.bufferOffset					   = command.TextureBufferCopy.BufferOffset;
			copyRegion.bufferRowLength				   = 0;
			copyRegion.bufferImageHeight			   = 0;
			copyRegion.imageSubresource.aspectMask	   = aspectFlags;
			copyRegion.imageSubresource.mipLevel	   = command.TextureBufferCopy.TextureSubresource.MipLevel;
			copyRegion.imageSubresource.layerCount	   = command.TextureBufferCopy.TextureSubresource.Depth;
			copyRegion.imageOffset.x				   = command.TextureBufferCopy.TextureSubresource.X;
			copyRegion.imageOffset.y				   = command.TextureBufferCopy.TextureSubresource.Y;
			copyRegion.imageOffset.z				   = command.TextureBufferCopy.TextureSubresource.Z;
			copyRegion.imageExtent.width			   = command.TextureBufferCopy.TextureSubresource.Width;
			copyRegion.imageExtent.height			   = command.TextureBufferCopy.TextureSubresource.Height;
			copyRegion.imageExtent.depth			   = command.TextureBufferCopy.TextureSubresource.Depth;
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
			copyRegion.srcSubresource.aspectMask	 = srcAspect;
			copyRegion.srcSubresource.mipLevel		 = command.TextureCopy.SourceSubresource.MipLevel;
			copyRegion.srcSubresource.layerCount	 = command.TextureCopy.SourceSubresource.Depth;
			copyRegion.srcOffset.x					 = command.TextureCopy.SourceSubresource.X;
			copyRegion.srcOffset.y					 = command.TextureCopy.SourceSubresource.Y;
			copyRegion.srcOffset.z					 = command.TextureCopy.SourceSubresource.Z;
			copyRegion.srcSubresource.baseArrayLayer = command.TextureCopy.SourceSubresource.ArrayLayer;
			copyRegion.extent.width					 = command.TextureCopy.SourceSubresource.Width;
			copyRegion.extent.height				 = command.TextureCopy.SourceSubresource.Height;
			copyRegion.extent.depth					 = command.TextureCopy.SourceSubresource.Depth;

			// dst
			copyRegion.dstSubresource.aspectMask	 = dstAspect;
			copyRegion.dstSubresource.mipLevel		 = command.TextureCopy.DestinationSubresource.MipLevel;
			copyRegion.dstSubresource.layerCount	 = command.TextureCopy.DestinationSubresource.Depth;
			copyRegion.dstOffset.x					 = command.TextureCopy.DestinationSubresource.X;
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

	void CommandExecutorVk::ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device)
	{
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		if (functions.vkCmdBeginDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.GroupName.c_str();
			labelEXT.color[0]			  = 0;
			labelEXT.color[1]			  = 0;
			labelEXT.color[2]			  = 0;
			labelEXT.color[3]			  = 0;
			functions.vkCmdBeginDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (functions.vkCmdDebugMarkerBeginEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pMarkerName				  = command.GroupName.c_str();
			functions.vkCmdDebugMarkerBeginEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device)
	{
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		// if this is the last command in the buffer, then we must explicitly stop rendering to ensure that the implict render pass management occurs
		// in the correct order
		if (m_CurrentCommandIndex >= m_Commands.size() - 1)
		{
			StopRendering();
		}
		// otherwise, if the next command is to set a new render target, we need to stop rendering to ensure that they show in the correct order in
		// debuggers
		else
		{
			RenderCommandData data = m_Commands.at(m_CurrentCommandIndex);
			if (std::holds_alternative<RenderTarget>(data))
			{
				StopRendering();
			}
		}

		if (functions.vkCmdEndDebugUtilsLabelEXT)
		{
			functions.vkCmdEndDebugUtilsLabelEXT(m_CommandBuffer);
		}
		else if (functions.vkCmdDebugMarkerEndEXT)
		{
			functions.vkCmdDebugMarkerEndEXT(m_CommandBuffer);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device)
	{
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		if (functions.vkCmdInsertDebugUtilsLabelEXT)
		{
			VkDebugUtilsLabelEXT labelEXT = {};
			labelEXT.sType				  = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelEXT.pNext				  = nullptr;
			labelEXT.pLabelName			  = command.MarkerName.c_str();
			labelEXT.color[0]			  = 0;
			labelEXT.color[1]			  = 0;
			labelEXT.color[2]			  = 0;
			labelEXT.color[3]			  = 0;
			functions.vkCmdInsertDebugUtilsLabelEXT(m_CommandBuffer, &labelEXT);
		}
		else if (functions.vkCmdDebugMarkerInsertEXT)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType					  = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			markerInfo.pMarkerName				  = command.MarkerName.c_str();
			functions.vkCmdDebugMarkerInsertEXT(m_CommandBuffer, &markerInfo);
		}
	}

	void CommandExecutorVk::ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device)
	{
		float blendConstants[4] = {command.BlendFactorDesc.Red,
								   command.BlendFactorDesc.Green,
								   command.BlendFactorDesc.Blue,
								   command.BlendFactorDesc.Alpha};

		vkCmdSetBlendConstants(m_CommandBuffer, blendConstants);
	}

	void CommandExecutorVk::ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device)
	{
		vkCmdSetStencilReference(m_CommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, command.StencilReference);
	}

	void CommandExecutorVk::ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device)
	{
		// return early if the function is not available to use
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
		if (!functions.vkCmdBuildAccelerationStructuresKHR)
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
		functions.vkCmdBuildAccelerationStructuresKHR(m_CommandBuffer,
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

	void BeginRenderPass(GraphicsDeviceVk			 *device,
						 const VkRenderPassBeginInfo &beginInfo,
						 VkSubpassContents			  subpassContents,
						 VkCommandBuffer			  commandBuffer)
	{
		const VulkanDeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		if (functions.vkCmdBeginRenderPass2KHR)
		{
			VkSubpassBeginInfo subpassInfo = {};
			subpassInfo.sType			   = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
			subpassInfo.pNext			   = nullptr;
			subpassInfo.contents		   = subpassContents;

			functions.vkCmdBeginRenderPass2KHR(commandBuffer, &beginInfo, &subpassInfo);
		}
		else
		{
			vkCmdBeginRenderPass(commandBuffer, &beginInfo, subpassContents);
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
			device->TransitionVulkanImageLayout(commandBuffer,
												swapchain->GetResolveImage(),
												0,
												0,
												swapchain->GetResolveImageLayout(),
												VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
												VK_IMAGE_ASPECT_COLOR_BIT);
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

		const VulkanDeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		functions.vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);
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

		if (swapchainDesc.Samples != 1)
		{
			m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
												  swapchainVk->GetResolveImage(),
												  0,
												  0,
												  swapchainVk->GetResolveImageLayout(),
												  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
												  VK_IMAGE_ASPECT_COLOR_BIT);
			swapchainVk->SetResolveImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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
		VkRect2D renderArea {};
		renderArea.offset = {0, 0};
		renderArea.extent = {framebuffer->GetFramebufferSpecification().Width, framebuffer->GetFramebufferSpecification().Height};

		std::vector<VkRenderingAttachmentInfo> colourAttachments;

		// attach colour textures
		for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < framebuffer->GetColorTextureCount(); colourAttachmentIndex++)
		{
			Ref<TextureVk> texture = framebuffer->GetVulkanColorTexture(colourAttachmentIndex);

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
			Ref<TextureVk> texture = framebuffer->GetVulkanDepthTexture();

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

		const VulkanDeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		functions.vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);
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

		// transition colour layouts
		for (size_t textureIndex = 0; textureIndex < vulkanFramebuffer->GetColorTextureCount(); textureIndex++)
		{
			Ref<TextureVk> texture = vulkanFramebuffer->GetVulkanColorTexture(textureIndex);

			for (uint32_t mipLevel = 0; mipLevel < texture->GetDescription().MipLevels; mipLevel++)
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

			for (uint32_t mipLevel = 0; mipLevel < texture->GetDescription().MipLevels; mipLevel++)
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
		if (m_Rendering)
		{
			const VulkanDeviceFeatures &features = m_Device->GetDeviceFeatures();
			if (features.DynamicRenderingAvailable)
			{
				const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
				functions.vkCmdEndRenderingKHR(m_CommandBuffer);
			}
			else
			{
				vkCmdEndRenderPass(m_CommandBuffer);
			}

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

			for (uint32_t level = 0; level < texture->GetDescription().MipLevels; level++)
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