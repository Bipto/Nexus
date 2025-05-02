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
		drawCall(pipeline);
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
									   0,
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
				uint32_t		 offset			  = command.IndexStart * indexSizeInBytes + indexBufferView.Offset;
				GLenum			 indexFormat	  = GL::GetGLIndexBufferFormat(indexBufferView.BufferFormat);

				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   0,
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
				DeviceBufferOpenGL *indirectBuffer = (DeviceBufferOpenGL *)command.IndirectBuffer;
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetBufferHandle());

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

				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
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
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		pipeline->Bind();
		BindResourceSet(m_BoundResourceSet);
		glDispatchCompute(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchIndirectCommand command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		pipeline->Bind();
		BindResourceSet(m_BoundResourceSet);

		DeviceBufferOpenGL *indirectBuffer = (DeviceBufferOpenGL *)command.IndirectBuffer;
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, indirectBuffer->GetBufferHandle());
		glDispatchComputeIndirect(command.Offset);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
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

		float color[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};

		glCall(glClearBufferfv(GL_COLOR, command.Index, color));
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
			if (Nexus::Graphics::Swapchain **previousSwapchain = m_CurrentRenderTarget.value().GetDataIf<Swapchain *>())
			{
				deviceGL->GetOffscreenContext()->MakeCurrent();
			}
		}

		// handle different options of render targets
		if (Nexus::Graphics::Swapchain **swapchain = command.GetDataIf<Swapchain *>())
		{
			deviceGL->SetSwapchain(*swapchain);
		}

		if (Nexus::Ref<Nexus::Graphics::Framebuffer> *framebuffer = command.GetDataIf<Nexus::Ref<Nexus::Graphics::Framebuffer>>())
		{
			deviceGL->SetFramebuffer(*framebuffer);
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

		Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(command.Source.lock());
		SwapchainOpenGL		  *swapchain   = (SwapchainOpenGL *)command.Target;

		framebuffer->BindAsReadBuffer(command.SourceIndex);
		swapchain->BindAsDrawTarget();

		uint32_t framebufferWidth  = framebuffer->GetFramebufferSpecification().Width;
		uint32_t framebufferHeight = framebuffer->GetFramebufferSpecification().Height;

		Ref<Texture2D> framebufferTexture = framebuffer->GetColorTexture(command.SourceIndex);

		Nexus::IWindow *window			= swapchain->GetWindow();
		uint32_t		swapchainWidth	= window->GetWindowSize().X;
		uint32_t		swapchainHeight = window->GetWindowSize().Y;

		glCall(glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, swapchainWidth, swapchainHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
	}

	void CommandExecutorOpenGL::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		if (command.Query.expired())
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query.lock());

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
		if (command.Query.expired())
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query.lock());

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

	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
	{
		DeviceBufferOpenGL *src = (DeviceBufferOpenGL *)command.BufferCopy.Source;
		DeviceBufferOpenGL *dst = (DeviceBufferOpenGL *)command.BufferCopy.Destination;

		glBindBuffer(GL_COPY_READ_BUFFER, src->GetBufferHandle());
		glBindBuffer(GL_COPY_WRITE_BUFFER, dst->GetBufferHandle());

		glCopyBufferSubData(GL_COPY_READ_BUFFER,
							GL_COPY_WRITE_BUFFER,
							command.BufferCopy.ReadOffset,
							command.BufferCopy.WriteOffset,
							command.BufferCopy.Size);

		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
	{
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

			if (combinedImageSampler.ImageSampler.expired())
			{
				NX_ERROR("Attempting to bind an invalid sampler");
				valid = false;
			}

			if (!valid)
			{
				continue;
			}

			Ref<SamplerOpenGL> glSampler = std::dynamic_pointer_cast<SamplerOpenGL>(combinedImageSampler.ImageSampler.lock());

			// find the slot in the shader where the uniform is located
			GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				// bind texture2D if needed
				if (std::holds_alternative<WeakRef<Texture2D>>(combinedImageSampler.ImageTexture))
				{
					WeakRef<Texture2D> texture = std::get<WeakRef<Texture2D>>(combinedImageSampler.ImageTexture);
					if (Ref<Texture2DOpenGL> glTexture = std::dynamic_pointer_cast<Texture2DOpenGL>(texture.lock()))
					{
						glTexture->Bind(location);
						glSampler->Bind(location, glTexture->GetLevels() > 1);
					}
				}

				// bind cubemap if needed
				else if (std::holds_alternative<WeakRef<Cubemap>>(combinedImageSampler.ImageTexture))
				{
					WeakRef<Cubemap> cubemap = std::get<WeakRef<Cubemap>>(combinedImageSampler.ImageTexture);
					if (Ref<CubemapOpenGL> glCubemap = std::dynamic_pointer_cast<CubemapOpenGL>(cubemap.lock()))
					{
						glCubemap->Bind(location);
						glSampler->Bind(location, glCubemap->GetLevels() > 1);
					}
				}

				else
				{
					throw std::runtime_error("Attempting to bind invalid texture type");
				}
			}
		}

		GLuint uniformBufferSlot = 0;
		for (const auto [name, uniformBufferView] : uniformBufferBindings)
		{
			DeviceBufferOpenGL *uniformBufferGL = (DeviceBufferOpenGL *)uniformBufferView.BufferHandle;

			GLint location = glGetUniformBlockIndex(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				glCall(glUniformBlockBinding(pipeline->GetShaderHandle(), location, uniformBufferSlot));

				glCall(glBindBufferRange(GL_UNIFORM_BUFFER,
										 uniformBufferSlot,
										 uniformBufferGL->GetBufferHandle(),
										 uniformBufferView.Offset,
										 uniformBufferView.Size));

				uniformBufferSlot++;
			}
		}

		for (const auto [name, storageImageView] : storageImageBindings)
		{
			GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				Texture2DOpenGL *texture = (Texture2DOpenGL *)storageImageView.TextureHandle;
				GLenum			 format	 = GL::GetSizedInternalFormat(storageImageView.TextureHandle->GetSpecification().Format, false);
				GLenum			 access	 = GL::GetAccessMask(storageImageView.Access);

				glCall(glBindImageTexture(location, texture->GetHandle(), storageImageView.Level, GL_FALSE, 0, access, format));
			}
		}
	}
}	 // namespace Nexus::Graphics

#endif