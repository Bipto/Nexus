#if defined(NX_PLATFORM_OPENGL)

	#include "CommandExecutorOpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"

namespace Nexus::Graphics
{
	CommandExecutorOpenGL::~CommandExecutorOpenGL()
	{
		Reset();
	}

	void CommandExecutorOpenGL::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device)
	{
		for (const auto &element : commands)
		{
			std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, element);
		}
	}

	void CommandExecutorOpenGL::Reset()
	{
		m_CurrentlyBoundPipeline	  = {};
		m_CurrentlyBoundVertexBuffers = {};
		m_CurrentRenderTarget		  = {};
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CurrentlyBoundVertexBuffers[command.Slot] = command.View;
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_BoundIndexBuffer = command.View;
	}

	void CommandExecutorOpenGL::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
		if (command.expired())
		{
			NX_ERROR("Attempting to bind an invalid pipeline");
			return;
		}

		if (Ref<Pipeline> pipeline = std::dynamic_pointer_cast<Pipeline>(command.lock()))
		{
			m_CurrentlyBoundPipeline = pipeline;
		}
	}

	void CommandExecutorOpenGL::ExecuteGraphicsCommand(Ref<GraphicsPipelineOpenGL>									pipeline,
													   const std::map<uint32_t, Nexus::Graphics::VertexBufferView> &vertexBuffers,
													   std::optional<Nexus::Graphics::IndexBufferView>				indexBuffer,
													   uint32_t														vertexOffset,
													   uint32_t														instanceOffset,
													   std::function<void(Ref<GraphicsPipelineOpenGL> pipeline)>	drawCall)
	{
		pipeline->CreateVAO();
		pipeline->BindBuffers(vertexBuffers, indexBuffer, vertexOffset, instanceOffset);
		pipeline->Bind();
		BindResourceSet(m_BoundResourceSet);

		bool valid = true;
		for (const auto &[binding, view] : vertexBuffers)
		{
			if (view.Stride == 0 || view.Size == 0)
				valid = false;
		}

		if (valid)
		{
			drawCall(pipeline);
		}

		pipeline->DestroyVAO();
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.has_value())
		{
			Ref<Pipeline> pipeline = m_CurrentlyBoundPipeline.value();
			if (pipeline->GetType() == PipelineType::Graphics)
			{
				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   command.VertexStart,
									   command.InstanceStart,
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline)
									   {
										   glCall(glDrawArraysInstanced(GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology),
																		command.VertexStart,
																		command.VertexCount,
																		command.InstanceCount));
									   });
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.has_value())
		{
			Ref<Pipeline> pipeline = m_CurrentlyBoundPipeline.value();

			if (pipeline->GetType() == PipelineType::Graphics && m_BoundIndexBuffer)
			{
				IndexBufferView &indexBufferView  = m_BoundIndexBuffer.value();
				uint32_t		 indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
				uint32_t		 offset			  = (command.IndexStart * indexSizeInBytes) + indexBufferView.Offset;
				GLenum			 indexFormat	  = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   command.VertexStart,
									   command.InstanceStart,
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline)
									   {
										   glCall(
											   glDrawElementsInstanced(GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology),
																	   command.IndexCount,
																	   indexFormat,
																	   (void *)(uint64_t)offset,
																	   command.InstanceCount));
									   });
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.has_value())
		{
			Ref<Pipeline> pipeline = m_CurrentlyBoundPipeline.value();
			if (pipeline->GetType() == PipelineType::Graphics)
			{
	#if !defined(__EMSCRIPTEN__)
				Ref<DeviceBufferOpenGL> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.IndirectBuffer);
				indirectBuffer->Bind(GL_DRAW_INDIRECT_BUFFER);
				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   0,
									   0,
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline)
									   {
										   uint32_t indirectOffset = command.Offset;
										   for (uint32_t i = 0; i < command.DrawCount; i++)
										   {
											   glDrawArraysIndirect(GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology),
																	(const void *)(uint64_t)indirectOffset);
										   }
									   });
				GL::ClearBufferBinding(GL_DRAW_INDIRECT_BUFFER);
	#endif
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndirectIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (m_CurrentlyBoundPipeline.has_value())
		{
			Ref<Pipeline> pipeline = m_CurrentlyBoundPipeline.value();

			if (pipeline->GetType() == PipelineType::Graphics && m_BoundIndexBuffer)
			{
				IndexBufferView &indexBufferView  = m_BoundIndexBuffer.value();
				uint32_t		 indexSizeInBytes = Graphics::GetIndexFormatSizeInBytes(indexBufferView.BufferFormat);
				GLenum			 indexFormat	  = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

	#if !defined(__EMSCRIPTEN__)
				Ref<DeviceBufferOpenGL> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.IndirectBuffer);
				indirectBuffer->Bind(GL_DRAW_INDIRECT_BUFFER);

				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   0,
									   0,
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline)
									   {
										   uint32_t indirectOffset = command.Offset;
										   for (uint32_t i = 0; i < command.DrawCount; i++)
										   {
											   glDrawElementsIndirect(GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology),
																	  indexFormat,
																	  (const void *)(uint64_t)indirectOffset);
										   }
									   });

				GL::ClearBufferBinding(GL_DRAW_INDIRECT_BUFFER);
	#endif
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

	#if !defined(__EMSCRIPTEN__)
		Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		pipeline->Bind();
		BindResourceSet(m_BoundResourceSet);
		glDispatchCompute(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

	#if !defined(__EMSCRIPTEN__)
		Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		pipeline->Bind();
		BindResourceSet(m_BoundResourceSet);

		if (Ref<DeviceBuffer> buffer = command.IndirectBuffer)
		{
			Ref<DeviceBufferOpenGL> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(buffer);
			indirectBuffer->Bind(GL_DISPATCH_INDIRECT_BUFFER);
			glDispatchComputeIndirect(command.Offset);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			GL::ClearBufferBinding(GL_DISPATCH_INDIRECT_BUFFER);
		}
	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		if (!command)
		{
			NX_ERROR("Attempting to update pipeline with invalid resources");
			return;
		}

		Ref<ResourceSetOpenGL> resourceSet = std::dynamic_pointer_cast<ResourceSetOpenGL>(command);
		m_BoundResourceSet				   = resourceSet;
	}

	void CommandExecutorOpenGL::ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearColour(m_CurrentRenderTarget, command.Index))
		{
			return;
		}

		if (command.Rect.has_value())
		{
			Graphics::ClearRect rect = command.Rect.value();

			GLint scissorBox[4];
			glGetIntegerv(GL_SCISSOR_BOX, scissorBox);

			RenderTarget target	  = m_CurrentRenderTarget.value();
			float		 scissorY = target.GetSize().Y - rect.Height - rect.Y;
			glCall(glScissor(rect.X, scissorY, rect.Width, rect.Height));

			float color[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
			glCall(glClearBufferfv(GL_COLOR, command.Index, color));

			glCall(glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]));
		}
		else
		{
			float color[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
			glCall(glClearBufferfv(GL_COLOR, command.Index, color));
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearDepth(m_CurrentRenderTarget))
		{
			return;
		}

		GLfloat depth	= command.Value.Depth;
		GLint	stencil = command.Value.Stencil;

		// enable clearing of depth buffer
		glDepthMask(GL_TRUE);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
		glDepthMask(GL_FALSE);
	}

	void CommandExecutorOpenGL::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;

		if (m_CurrentRenderTarget.has_value())
		{
			RenderTarget target = m_CurrentRenderTarget.value();
			if (target.GetType() == RenderTargetType::Swapchain)
			{
				Nexus::GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();
				offscreenContext->MakeCurrent();
			}
		}

		if (command.GetType() == RenderTargetType::Framebuffer)
		{
			WeakRef<Framebuffer> fb = command.GetFramebuffer();
			deviceGL->SetFramebuffer(fb);
		}
		else if (command.GetType() == RenderTargetType::Swapchain)
		{
			WeakRef<Swapchain> sc = command.GetSwapchain();
			deviceGL->SetSwapchain(sc);
		}
		else
		{
			throw std::runtime_error("Failed to find a valid render target type");
		}

		m_CurrentRenderTarget = command;
	}

	void CommandExecutorOpenGL::ExecuteCommand(const Viewport &command, GraphicsDevice *device)
	{
		if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
		{
			return;
		}

		RenderTarget target = m_CurrentRenderTarget.value();

		float left	 = command.X;
		float bottom = target.GetSize().Y - (command.Y + command.Height);

		glCall(glViewport(left, bottom, command.Width, command.Height));

		glCall(glDepthRangef(command.MinDepth, command.MaxDepth));
	}

	void CommandExecutorOpenGL::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
	{
		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		RenderTarget target	  = m_CurrentRenderTarget.value();
		float		 scissorY = target.GetSize().Y - command.Height - command.Y;

		glCall(glScissor(command.X, scissorY, command.Width, command.Height));
	}

	void CommandExecutorOpenGL::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
	{
		if (!ValidateForResolveToSwapchain(command))
		{
			return;
		}

		Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(command.Source);
		Ref<SwapchainOpenGL>   swapchain   = std::dynamic_pointer_cast<SwapchainOpenGL>(command.Target);

		framebuffer->BindAsReadBuffer(command.SourceIndex);
		swapchain->BindAsDrawTarget();

		uint32_t framebufferWidth  = framebuffer->GetFramebufferSpecification().Width;
		uint32_t framebufferHeight = framebuffer->GetFramebufferSpecification().Height;

		Ref<Texture> framebufferTexture = framebuffer->GetColorTexture(command.SourceIndex);

		Nexus::IWindow *window			= swapchain->GetWindow();
		uint32_t		swapchainWidth	= window->GetWindowSize().X;
		uint32_t		swapchainHeight = window->GetWindowSize().Y;

		glCall(glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, swapchainWidth, swapchainHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
	}

	void CommandExecutorOpenGL::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		if (!command.Query)
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query);

	#if defined(__EMSCRIPTEN__) || defined(ANDROID)
		glFinish();
		uint64_t now   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		query->m_Start = now;
	#else
		glFinish();
		GLint64 timer;
		glCall(glGetInteger64v(GL_TIMESTAMP, &timer));
		query->m_Start = (uint64_t)timer;
	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		if (!command.Query)
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query);

	#if defined(__EMSCRIPTEN__) || defined(ANDROID)
		glFinish();
		uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		query->m_End = now;
	#else
		glFinish();
		GLint64 timer;
		glCall(glGetInteger64v(GL_TIMESTAMP, &timer));
		query->m_End = (uint64_t)timer;
	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		glStencilMask(command.Value);
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		glDepthRangef(command.Min, command.Max);
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		glBlendColor(command.R, command.G, command.B, command.A);
	}

	void CommandExecutorOpenGL::ExecuteCommand(const BarrierDesc &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.BufferTextureCopy.TextureHandle);

		GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;
		deviceGL->CopyBufferToTexture(textureOpenGL, buffer, command.BufferTextureCopy.BufferOffset, command.BufferTextureCopy.TextureSubresource);
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureBufferCopy.TextureHandle);

		GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;
		deviceGL->CopyTextureToBuffer(textureOpenGL, buffer, command.TextureBufferCopy.BufferOffset, command.TextureBufferCopy.TextureSubresource);
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<TextureOpenGL>			  sourceTexture = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Source);
		Ref<TextureOpenGL>			  destTexture	= std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Destination);
		const TextureCopyDescription &copyDesc		= command.TextureCopy;

		GLenum srcGlAspect		 = GL::GetGLImageAspect(command.TextureCopy.SourceSubresource.Aspect);
		GLenum srcAttachmentType = GL::GetAttachmentType(command.TextureCopy.SourceSubresource.Aspect, 0);
		GLenum dstGlAspect		 = GL::GetGLImageAspect(command.TextureCopy.DestinationSubresource.Aspect);
		GLenum dstAttachmentType = GL::GetAttachmentType(command.TextureCopy.DestinationSubresource.Aspect, 0);

		for (uint32_t layer = command.TextureCopy.SourceSubresource.Z; layer < command.TextureCopy.SourceSubresource.Depth; layer++)
		{
			GLuint sourceFramebufferHandle = 0;
			GLuint destFramebufferHandle   = 0;

			// set up source framebuffer
			{
				glCall(glGenFramebuffers(1, &sourceFramebufferHandle));
				glCall(glBindFramebuffer(GL_FRAMEBUFFER, sourceFramebufferHandle));
				GLenum aspectMask = GL::GetGLImageAspect(command.TextureCopy.SourceSubresource.Aspect);
				GL::AttachTexture(sourceFramebufferHandle,
								  sourceTexture,
								  command.TextureCopy.SourceSubresource.MipLevel,
								  layer,
								  copyDesc.SourceSubresource.Aspect,
								  0);
				GL::ValidateFramebuffer(sourceFramebufferHandle);
			}

			// set up dest framebuffer
			{
				glCall(glGenFramebuffers(1, &destFramebufferHandle));
				glCall(glBindFramebuffer(GL_FRAMEBUFFER, destFramebufferHandle));
				GLenum aspectMask = GL::GetGLImageAspect(command.TextureCopy.DestinationSubresource.Aspect);
				GL::AttachTexture(destFramebufferHandle,
								  destTexture,
								  command.TextureCopy.DestinationSubresource.MipLevel,
								  layer,
								  copyDesc.DestinationSubresource.Aspect,
								  0);
				GL::ValidateFramebuffer(destFramebufferHandle);
			}

			glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFramebufferHandle);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destFramebufferHandle);

			// copy all attached aspect masks
			glBlitFramebuffer(command.TextureCopy.SourceSubresource.X,
							  command.TextureCopy.SourceSubresource.Y,
							  command.TextureCopy.SourceSubresource.Width,
							  command.TextureCopy.SourceSubresource.Height,
							  command.TextureCopy.DestinationSubresource.X,
							  command.TextureCopy.DestinationSubresource.Y,
							  command.TextureCopy.DestinationSubresource.Width,
							  command.TextureCopy.DestinationSubresource.Height,
							  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
							  GL_NEAREST);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glCall(glDeleteFramebuffers(1, &sourceFramebufferHandle));
			glCall(glDeleteFramebuffers(1, &destFramebufferHandle));
		}
	}

	void CommandExecutorOpenGL::BindResourceSet(Ref<ResourceSetOpenGL> resourceSet)
	{
		Nexus::Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		if (!pipeline)
			return;

		pipeline->Bind();

		const auto &combinedImageSamplers = resourceSet->GetBoundCombinedImageSamplers();
		const auto &uniformBufferBindings = resourceSet->GetBoundUniformBuffers();
		const auto &storageImageBindings  = resourceSet->GetBoundStorageImages();

		for (const auto [name, combinedImageSampler] : combinedImageSamplers)
		{
			bool valid = true;

			if (!combinedImageSampler.ImageSampler)
			{
				NX_ERROR("Attempting to bind an invalid sampler");
				valid = false;
			}

			if (!valid)
			{
				continue;
			}

			Ref<SamplerOpenGL> glSampler = std::dynamic_pointer_cast<SamplerOpenGL>(combinedImageSampler.ImageSampler);

			// find the slot in the shader where the uniform is located
			GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				if (Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(combinedImageSampler.ImageTexture))
				{
					texture->Bind(location);
					glSampler->Bind(location, texture->GetSpecification().MipLevels > 1);
				}
			}
		}

		GLuint uniformBufferSlot = 0;
		for (const auto [name, uniformBufferView] : uniformBufferBindings)
		{
			Ref<DeviceBufferOpenGL> uniformBufferGL = std::dynamic_pointer_cast<DeviceBufferOpenGL>(uniformBufferView.BufferHandle);

			GLint location = glGetUniformBlockIndex(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				glCall(glUniformBlockBinding(pipeline->GetShaderHandle(), location, uniformBufferSlot));

				uniformBufferGL->BindRange(GL_UNIFORM_BUFFER, uniformBufferSlot, uniformBufferView.Offset, uniformBufferView.Size);

				uniformBufferSlot++;
			}
		}

		for (const auto [name, storageImageView] : storageImageBindings)
		{
			GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
	#if !defined(__EMSCRIPTEN__)
				Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(storageImageView.TextureHandle);
				GLenum			   format  = GL::GetSizedInternalFormat(storageImageView.TextureHandle->GetSpecification().Format, false);
				GLenum			   access  = GL::GetAccessMask(storageImageView.Access);

				GLboolean layered = GL_FALSE;
				if (texture->GetSpecification().ArrayLayers > 1)
				{
					layered = GL_TRUE;
				}

				glCall(glBindImageTexture(location,
										  texture->GetHandle(),
										  storageImageView.MipLevel,
										  layered,
										  storageImageView.ArrayLayer,
										  access,
										  format));
	#endif
			}
		}
	}
}	 // namespace Nexus::Graphics

#endif