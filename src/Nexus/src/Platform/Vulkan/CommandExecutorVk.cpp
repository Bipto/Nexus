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

		DeviceBufferVk *vertexBufferVk	= (DeviceBufferVk *)command.View.BufferHandle;
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
		DeviceBufferVk *indexBufferVk	  = (DeviceBufferVk *)command.View.BufferHandle;
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
		DeviceBufferVk *indirectBuffer = (DeviceBufferVk *)command.IndirectBuffer;

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
		DeviceBufferVk *indirectBuffer = (DeviceBufferVk *)command.IndirectBuffer;

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

		DeviceBufferVk *indirectBuffer = (DeviceBufferVk *)command.IndirectBuffer;

		vkCmdDispatchIndirect(m_CommandBuffer, indirectBuffer->GetVkBuffer(), command.Offset);
	}

	void CommandExecutorVk::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		auto pipeline	   = std::dynamic_pointer_cast<PipelineVk>(m_CurrentlyBoundPipeline);
		auto resourceSetVk = std::dynamic_pointer_cast<ResourceSetVk>(command);
		pipeline->SetResourceSet(m_CommandBuffer, resourceSetVk.get());
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
			auto swapchain		 = m_CurrentRenderTarget.GetData<Swapchain *>();
			auto vulkanSwapchain = (SwapchainVk *)swapchain;

			StartRenderingToSwapchain(vulkanSwapchain);
		}
		else
		{
			auto framebuffer	   = m_CurrentRenderTarget.GetData<Ref<Framebuffer>>();
			auto vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

			StartRenderingToFramebuffer(vulkanFramebuffer);
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

		auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(command.Source.lock());
		auto swapchainVk   = (SwapchainVk *)command.Target;

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

		auto framebufferLayout = framebufferVk->GetVulkanColorTexture(command.SourceIndex)->GetImageLayout(0);
		auto swapchainLayout   = swapchainVk->GetColorImageLayout();

		vkCmdResolveImage(m_CommandBuffer, framebufferImage, framebufferLayout, swapchainImage, swapchainLayout, 1, &resolve);

		ExecuteCommand(m_CurrentRenderTarget, device);
	}

	void CommandExecutorVk::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query.lock());
		vkCmdResetQueryPool(m_CommandBuffer, queryVk->GetQueryPool(), 0, 2);
		vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryVk->GetQueryPool(), 0);
	}

	void CommandExecutorVk::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		Ref<TimingQueryVk> queryVk = std::dynamic_pointer_cast<TimingQueryVk>(command.Query.lock());
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

	void CommandExecutorVk::StartRenderingToSwapchain(SwapchainVk *swapchain)
	{
		m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
											  swapchain->GetColourImage(),
											  0,
											  0,
											  swapchain->GetColorImageLayout(),
											  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
											  VK_IMAGE_ASPECT_COLOR_BIT);

		m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
											  swapchain->GetDepthImage(),
											  0,
											  0,
											  swapchain->GetDepthImageLayout(),
											  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
											  VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));

		swapchain->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		swapchain->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		VkExtent2D swapchainSize = swapchain->GetSwapchainSize();

		VkRect2D renderArea;
		renderArea.offset = {0, 0};
		renderArea.extent = swapchainSize;

		VkRenderingAttachmentInfo colourAttachment = {};
		colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;

		if (swapchain->GetSpecification().Samples == 1)
		{
			colourAttachment.imageView	 = swapchain->GetColourImageView();
			colourAttachment.imageLayout = swapchain->GetColorImageLayout();
		}
		else
		{
			m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
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

		vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
		m_Rendering = true;
	}

	void CommandExecutorVk::StartRenderingToFramebuffer(Ref<Framebuffer> framebuffer)
	{
		Ref<FramebufferVk> vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

		// transition colour layouts
		for (size_t textureIndex = 0; textureIndex < vulkanFramebuffer->GetColorTextureCount(); textureIndex++)
		{
			Ref<Texture2D_Vk> texture = vulkanFramebuffer->GetVulkanColorTexture(textureIndex);

			for (uint32_t mipLevel = 0; mipLevel < texture->GetLevels(); mipLevel++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  mipLevel,
													  0,
													  texture->GetImageLayout(mipLevel),
													  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
													  VK_IMAGE_ASPECT_COLOR_BIT);
				texture->SetImageLayout(mipLevel, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
		}

		// transition depth layout if needed
		if (vulkanFramebuffer->HasDepthTexture())
		{
			Ref<Texture2D_Vk> texture = vulkanFramebuffer->GetVulkanDepthTexture();

			for (uint32_t mipLevel = 0; mipLevel < texture->GetLevels(); mipLevel++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  mipLevel,
													  0,
													  texture->GetImageLayout(mipLevel),
													  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
													  VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
				texture->SetImageLayout(mipLevel, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			}
		}

		VkRect2D renderArea {};
		renderArea.offset = {0, 0};
		renderArea.extent = {vulkanFramebuffer->GetFramebufferSpecification().Width, vulkanFramebuffer->GetFramebufferSpecification().Height};

		std::vector<VkRenderingAttachmentInfo> colourAttachments;

		// attach colour textures
		for (uint32_t colourAttachmentIndex = 0; colourAttachmentIndex < vulkanFramebuffer->GetColorTextureCount(); colourAttachmentIndex++)
		{
			Ref<Texture2D_Vk> texture = vulkanFramebuffer->GetVulkanColorTexture(colourAttachmentIndex);

			VkRenderingAttachmentInfo colourAttachment = {};
			colourAttachment.sType					   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colourAttachment.imageView				   = texture->GetImageView();
			colourAttachment.imageLayout			   = texture->GetImageLayout(0);
			colourAttachment.loadOp					   = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp				   = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.clearValue				   = {};
			colourAttachments.push_back(colourAttachment);
		}

		// set up depth attachment (may be unused)
		VkRenderingAttachmentInfo depthAttachment = {};
		if (framebuffer->HasDepthTexture())
		{
			Ref<Texture2D_Vk> texture = vulkanFramebuffer->GetVulkanDepthTexture();

			depthAttachment.sType		= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView	= texture->GetImageView();
			depthAttachment.imageLayout = texture->GetImageLayout(0);
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

			if (Nexus::Ref<Nexus::Graphics::Framebuffer> *framebuffer = m_CurrentRenderTarget.GetDataIf<Ref<Framebuffer>>())
			{
				TransitionFramebufferToShaderReadonly(*framebuffer);
			}
		}

		m_Rendering = false;
	}

	void CommandExecutorVk::TransitionFramebufferToShaderReadonly(Ref<Framebuffer> framebuffer)
	{
		Ref<FramebufferVk> vulkanFramebuffer = std::dynamic_pointer_cast<FramebufferVk>(framebuffer);

		for (uint32_t colourAttachment = 0; colourAttachment < vulkanFramebuffer->GetColorTextureCount(); colourAttachment++)
		{
			Ref<Texture2D_Vk> texture = vulkanFramebuffer->GetVulkanColorTexture(colourAttachment);

			for (uint32_t level = 0; level < texture->GetLevels(); level++)
			{
				m_Device->TransitionVulkanImageLayout(m_CommandBuffer,
													  texture->GetImage(),
													  level,
													  0,
													  texture->GetImageLayout(level),
													  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
													  VK_IMAGE_ASPECT_COLOR_BIT);
				texture->SetImageLayout(level, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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