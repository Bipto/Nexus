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

	void CommandExecutorOpenGL::ExecuteGraphicsCommand(
		Ref<GraphicsPipelineOpenGL>																pipeline,
		const std::map<uint32_t, Nexus::Graphics::VertexBufferView>							   &vertexBuffers,
		std::optional<Nexus::Graphics::IndexBufferView>											indexBuffer,
		uint32_t																				vertexOffset,
		uint32_t																				instanceOffset,
		std::function<void(Ref<GraphicsPipelineOpenGL> pipeline, const GladGLContext &context)> drawCall)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				pipeline->CreateVAO(context);
				pipeline->BindBuffers(vertexBuffers, indexBuffer, vertexOffset, instanceOffset, context);
				pipeline->Bind(context);
				BindResourceSet(m_BoundResourceSet, context);

				bool valid = true;
				for (const auto &[binding, view] : vertexBuffers)
				{
					if (view.Size == 0)
						valid = false;
				}

				if (valid)
				{
					drawCall(pipeline, context);
				}

				pipeline->DestroyVAO(context);
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawDescription command, GraphicsDevice *device)
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
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline, const GladGLContext &context)
									   {
										   GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

										   if (command.InstanceCount == 1)
										   {
											   context.DrawArrays(topology, command.VertexStart, command.VertexCount);
										   }
										   else
										   {
											   context.DrawArraysInstanced(topology, command.VertexStart, command.VertexCount, command.InstanceCount);
										   }
									   });
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device)
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

				ExecuteGraphicsCommand(
					std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
					m_CurrentlyBoundVertexBuffers,
					m_BoundIndexBuffer,
					command.VertexStart,
					command.InstanceStart,
					[&](Ref<GraphicsPipelineOpenGL> graphicsPipeline, const GladGLContext &context)
					{
						GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

						if (command.InstanceCount == 1)
						{
							context.DrawElements(topology, command.IndexCount, indexFormat, (void *)(uint64_t)offset);
						}
						else
						{
							context.DrawElementsInstanced(topology, command.IndexCount, indexFormat, (void *)(uint64_t)offset, command.InstanceCount);
						}
					});
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device)
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

				ExecuteGraphicsCommand(
					std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
					m_CurrentlyBoundVertexBuffers,
					m_BoundIndexBuffer,
					0,
					0,
					[&](Ref<GraphicsPipelineOpenGL> graphicsPipeline, const GladGLContext &context)
					{
						context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());
						GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

						if (context.MultiDrawArraysIndirect)
						{
							context.MultiDrawArraysIndirect(topology, (const void *)(uint64_t)command.Offset, command.DrawCount, command.Stride);
						}
						else
						{
							uint32_t indirectOffset = command.Offset;
							for (uint32_t i = 0; i < command.DrawCount; i++)
							{
								context.DrawArraysIndirect(topology, (const void *)(uint64_t)indirectOffset);
							}
							indirectOffset += command.Stride;
						}

						context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
					});
	#endif
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device)
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

				ExecuteGraphicsCommand(std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline),
									   m_CurrentlyBoundVertexBuffers,
									   m_BoundIndexBuffer,
									   0,
									   0,
									   [&](Ref<GraphicsPipelineOpenGL> graphicsPipeline, const GladGLContext &context)
									   {
										   context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());
										   GLenum topology = GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology);

										   if (context.MultiDrawElementsIndirect)
										   {
											   context.MultiDrawElementsIndirect(topology,
																				 indexFormat,
																				 (const void *)(uint64_t)command.Offset,
																				 command.DrawCount,
																				 command.Stride);
										   }
										   else
										   {
											   uint32_t indirectOffset = command.Offset;
											   for (uint32_t i = 0; i < command.DrawCount; i++)
											   {
												   context.DrawElementsIndirect(
													   GL::GetTopology(graphicsPipeline->GetPipelineDescription().PrimitiveTopology),
													   indexFormat,
													   (const void *)(uint64_t)indirectOffset);
												   indirectOffset += command.Stride;
											   }
										   }

										   context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
									   });

	#endif
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
	#if !defined(__EMSCRIPTEN__)
				Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
				pipeline->Bind(context);
				BindResourceSet(m_BoundResourceSet, context);
				context.DispatchCompute(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
				context.MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

	#if !defined(__EMSCRIPTEN__)
		Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				pipeline->Bind(context);
				BindResourceSet(m_BoundResourceSet, context);

				if (Ref<DeviceBuffer> buffer = command.IndirectBuffer)
				{
					Ref<DeviceBufferOpenGL> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(buffer);
					context.BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, indirectBuffer->GetHandle());
					context.DispatchComputeIndirect(command.Offset);
					context.MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
					context.BindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
				}
			});

	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawMeshDescription command, GraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawMeshIndirectDescription command, GraphicsDevice *device)
	{
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

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (command.Rect.has_value())
				{
					Graphics::ClearRect rect = command.Rect.value();

					GLint scissorBox[4];
					context.GetIntegerv(GL_SCISSOR_BOX, scissorBox);

					RenderTarget target	  = m_CurrentRenderTarget.value();
					float		 scissorY = target.GetSize().Y - rect.Height - rect.Y;
					glCall(context.Scissor(rect.X, scissorY, rect.Width, rect.Height));

					float color[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
					glCall(context.ClearBufferfv(GL_COLOR, command.Index, color));

					glCall(context.Scissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]));
				}
				else
				{
					float color[] = {command.Color.Red, command.Color.Green, command.Color.Blue, command.Color.Alpha};
					glCall(context.ClearBufferfv(GL_COLOR, command.Index, color));
				}
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)
	{
		if (!ValidateForClearDepth(m_CurrentRenderTarget))
		{
			return;
		}

		GLfloat depth	= command.Value.Depth;
		GLint	stencil = command.Value.Stencil;

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				// enable clearing of depth buffer
				context.DepthMask(GL_TRUE);
				context.ClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
				context.DepthMask(GL_FALSE);
			});
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
				GL::SetCurrentContext(offscreenContext);
			}
		}

		if (command.GetType() == RenderTargetType::Framebuffer)
		{
			WeakRef<Framebuffer> fb = command.GetFramebuffer();
			if (Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(fb.lock()))
			{
				GL::ExecuteGLCommands([&](const GladGLContext &context) { framebuffer->BindAsDrawBuffer(context); });
			}
		}
		else if (command.GetType() == RenderTargetType::Swapchain)
		{
			WeakRef<Swapchain> sc = command.GetSwapchain();
			if (Ref<SwapchainOpenGL> swapchain = std::dynamic_pointer_cast<SwapchainOpenGL>(sc.lock()))
			{
				swapchain->BindAsDrawTarget();
				GL::SetCurrentContext(swapchain->GetViewContext());
			}
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

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				RenderTarget target = m_CurrentRenderTarget.value();

				float left	 = command.X;
				float bottom = target.GetSize().Y - (command.Y + command.Height);

				glCall(context.Viewport(left, bottom, command.Width, command.Height));
				glCall(context.DepthRangef(command.MinDepth, command.MaxDepth));
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const Scissor &command, GraphicsDevice *device)
	{
		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				RenderTarget target	  = m_CurrentRenderTarget.value();
				float		 scissorY = target.GetSize().Y - command.Height - command.Y;

				glCall(context.Scissor(command.X, scissorY, command.Width, command.Height));
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)
	{
		if (!ValidateForResolveToSwapchain(command))
		{
			return;
		}

		Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(command.Source);
		Ref<SwapchainOpenGL>   swapchain   = std::dynamic_pointer_cast<SwapchainOpenGL>(command.Target);

		swapchain->BindAsDrawTarget();
		GL::IViewContext *viewContext = swapchain->GetViewContext();
		GL::SetCurrentContext(viewContext);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				framebuffer->BindAsReadBuffer(command.SourceIndex, context);

				uint32_t framebufferWidth  = framebuffer->GetFramebufferSpecification().Width;
				uint32_t framebufferHeight = framebuffer->GetFramebufferSpecification().Height;

				Ref<Texture> framebufferTexture = framebuffer->GetColorTexture(command.SourceIndex);

				Nexus::IWindow *window			= swapchain->GetWindow();
				uint32_t		swapchainWidth	= window->GetWindowSize().X;
				uint32_t		swapchainHeight = window->GetWindowSize().Y;

				glCall(context.BlitFramebuffer(0,
											   0,
											   framebufferWidth,
											   framebufferHeight,
											   0,
											   0,
											   swapchainWidth,
											   swapchainHeight,
											   GL_COLOR_BUFFER_BIT,
											   GL_LINEAR));
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)
	{
		if (!command.Query)
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
	#if defined(__EMSCRIPTEN__) || defined(ANDROID)
				context.Finish();
				uint64_t now   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				query->m_Start = now;
	#else
				context.Finish();
				GLint64 timer;
				glCall(context.GetInteger64v(GL_TIMESTAMP, &timer));
				query->m_Start = (uint64_t)timer;
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)
	{
		if (!command.Query)
		{
			NX_ERROR("Attempting to write a timestamp to an invalid query object");
			return;
		}

		Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
	#if defined(__EMSCRIPTEN__) || defined(ANDROID)
				context.Finish();
				uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				query->m_End = now;
	#else
				context.Finish();
				GLint64 timer;
				glCall(context.GetInteger64v(GL_TIMESTAMP, &timer));
				query->m_End = (uint64_t)timer;
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.BufferTextureCopy.TextureHandle);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context) {
				GL::CopyBufferToTexture(textureOpenGL,
										buffer,
										command.BufferTextureCopy.BufferOffset,
										command.BufferTextureCopy.TextureSubresource,
										context);
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureBufferCopy.TextureHandle);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context) {
				GL::CopyTextureToBuffer(textureOpenGL,
										buffer,
										command.TextureBufferCopy.BufferOffset,
										command.TextureBufferCopy.TextureSubresource,
										context);
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)
	{
		Ref<TextureOpenGL>			  sourceTexture = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Source);
		Ref<TextureOpenGL>			  destTexture	= std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Destination);
		const TextureCopyDescription &copyDesc		= command.TextureCopy;

		GL::ExecuteGLCommands([&](const GladGLContext &context) { GL::CopyTextureToTexture(sourceTexture, destTexture, copyDesc, context); });
	}

	void CommandExecutorOpenGL::ExecuteCommand(BeginDebugGroupCommand command, GraphicsDevice *device)
	{
		// if glPushDebugGroup occurs on different contexts, we start getting errors about GL_STACK_OVERFLOW,
		// so we have to fix this by running all debug group functions from the GraphicsDevice context
		GL::IGLContext *previousContext = GL::GetCurrentContext();

		GraphicsDeviceOpenGL							 *deviceGL		 = (GraphicsDeviceOpenGL *)device;
		Nexus::Ref<Nexus::Graphics::PhysicalDeviceOpenGL> physicalDevice = deviceGL->GetPhysicalDeviceOpenGL();
		GL::SetCurrentContext(physicalDevice->GetOffscreenContext());

		GL::ExecuteGLCommands([&](const GladGLContext &context)
							  { glCall(context.PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, command.GroupName.c_str())); });

		GL::SetCurrentContext(previousContext);
	}

	void CommandExecutorOpenGL::ExecuteCommand(EndDebugGroupCommand command, GraphicsDevice *device)
	{
		// if glPushDebugGroup occurs on different contexts, we start getting errors about GL_STACK_OVERFLOW,
		// so we have to fix this by running all debug group functions from the GraphicsDevice context
		GL::IGLContext *previousContext = GL::GetCurrentContext();

		GraphicsDeviceOpenGL							 *deviceGL		 = (GraphicsDeviceOpenGL *)device;
		Nexus::Ref<Nexus::Graphics::PhysicalDeviceOpenGL> physicalDevice = deviceGL->GetPhysicalDeviceOpenGL();
		GL::SetCurrentContext(physicalDevice->GetOffscreenContext());

		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.PopDebugGroup(); });

		GL::SetCurrentContext(previousContext);
	}

	void CommandExecutorOpenGL::ExecuteCommand(InsertDebugMarkerCommand command, GraphicsDevice *device)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				context.DebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
										   GL_DEBUG_TYPE_MARKER,
										   0,
										   GL_DEBUG_SEVERITY_NOTIFICATION,
										   -1,
										   command.MarkerName.c_str());
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context) {
				context.BlendColor(command.BlendFactorDesc.Red,
								   command.BlendFactorDesc.Green,
								   command.BlendFactorDesc.Blue,
								   command.BlendFactorDesc.Alpha);
			});
	}

	void CommandExecutorOpenGL::BindResourceSet(Ref<ResourceSetOpenGL> resourceSet, const GladGLContext &context)
	{
		Nexus::Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		if (!pipeline)
			return;

		pipeline->Bind(context);

		const auto &combinedImageSamplers = resourceSet->GetBoundCombinedImageSamplers();
		const auto &uniformBufferBindings = resourceSet->GetBoundUniformBuffers();
		const auto &storageImageBindings  = resourceSet->GetBoundStorageImages();
		const auto &storageBufferBindings = resourceSet->GetBoundStorageBuffers();

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
			GLint location = context.GetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

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

			GLint location = context.GetUniformBlockIndex(pipeline->GetShaderHandle(), name.c_str());

			if (location != -1)
			{
				glCall(context.UniformBlockBinding(pipeline->GetShaderHandle(), location, uniformBufferSlot));
				context.BindBufferRange(GL_UNIFORM_BUFFER,
										uniformBufferSlot,
										uniformBufferGL->GetHandle(),
										uniformBufferView.Offset,
										uniformBufferView.Size);

				uniformBufferSlot++;
			}
		}

		for (const auto [name, storageImageView] : storageImageBindings)
		{
			GLint location = context.GetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

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

				glCall(context.BindImageTexture(location,
												texture->GetHandle(),
												storageImageView.MipLevel,
												layered,
												storageImageView.ArrayLayer,
												access,
												format));
	#endif
			}
		}

		GLuint storageBufferSlot = 0;
		for (const auto [name, storageBufferView] : storageBufferBindings)
		{
			// GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());
			GLuint location = context.GetProgramResourceIndex(pipeline->GetShaderHandle(), GL_SHADER_STORAGE_BLOCK, name.c_str());

			if (location != -1)
			{
	#if !defined(__EMSCRIPTEN__)
				Ref<DeviceBufferOpenGL> buffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(storageBufferView.BufferHandle);
				size_t					offset = storageBufferView.Offset;
				size_t					size   = storageBufferView.SizeInBytes;
				context.BindBufferRange(GL_SHADER_STORAGE_BUFFER, storageBufferSlot, buffer->GetHandle(), offset, size);

				storageBufferSlot++;
	#endif
			}
		}
	}
}	 // namespace Nexus::Graphics

#endif