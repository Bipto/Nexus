#if defined(NX_PLATFORM_OPENGL)

	#include "CommandExecutorOpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TextureViewOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"

	#include "Nexus-Core/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
	CommandExecutorOpenGL::~CommandExecutorOpenGL()
	{
		Reset();
	}

	void CommandExecutorOpenGL::ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device)
	{
		NX_PROFILE_FUNCTION();

		const std::vector<RenderCommandData> &commands = commandList->GetCommandData();
		for (const auto &command : commands)
		{
			std::visit([&](auto &&arg) { ExecuteCommand(arg, device); }, command);
		}
	}

	void CommandExecutorOpenGL::Reset()
	{
		m_CurrentlyBoundPipeline	  = {};
		m_CurrentlyBoundVertexBuffers = {};
		m_CurrentRenderTarget		  = {};
		m_BoundResourceSet			  = {};
	}

	void CommandExecutorOpenGL::ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_CurrentlyBoundVertexBuffers[command.Slot] = command.View;
	}

	void CommandExecutorOpenGL::ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		m_BoundIndexBuffer = command.View;
	}

	void CommandExecutorOpenGL::ExecuteCommand(WeakRef<Pipeline> command, IGraphicsDevice *device)
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
				BindResourceSet(context);

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

	void CommandExecutorOpenGL::ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)
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

	void CommandExecutorOpenGL::ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)
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

	void CommandExecutorOpenGL::ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)
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
							for (size_t i = 0; i < command.DrawCount; i++)
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

	void CommandExecutorOpenGL::ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)
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
											   size_t indirectOffset = command.Offset;
											   for (size_t i = 0; i < command.DrawCount; i++)
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

	void CommandExecutorOpenGL::ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)
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
				BindResourceSet(context);
				context.DispatchCompute(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
				context.MemoryBarrierEXT(GL_ALL_BARRIER_BITS);
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)
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
				BindResourceSet(context);

				if (Ref<IDeviceBuffer> buffer = command.IndirectBuffer)
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

	void CommandExecutorOpenGL::ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)
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
				BindResourceSet(context);

				if (context.DrawMeshTasksEXT)
				{
					context.DrawMeshTasksEXT(command.WorkGroupCountX, command.WorkGroupCountY, command.WorkGroupCountZ);
				}
			});

	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForComputeCall(m_CurrentlyBoundPipeline))
		{
			return;
		}

	#if !defined(__EMSCRIPTEN__)
		Ref<PipelineOpenGL>		pipeline	   = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		Ref<DeviceBufferOpenGL> indirectBuffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.IndirectBuffer);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				pipeline->Bind(context);
				BindResourceSet(context);

				context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->GetHandle());

				if (context.MultiDrawMeshTasksIndirectEXT)
				{
					context.MultiDrawMeshTasksIndirectEXT((GLintptr)command.Offset, command.DrawCount, command.Stride);
				}
				else if (context.DrawMeshTasksIndirectEXT)
				{
					size_t indirectOffset = command.Offset;
					for (uint32_t i = 0; i < command.DrawCount; i++) { context.DrawMeshTasksIndirectEXT((GLintptr)indirectOffset); }
					indirectOffset += command.Stride;
				}

				context.BindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
			});

	#endif
	}

	void CommandExecutorOpenGL::ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)
	{
		if (!desc.TargetResourceSet)
		{
			NX_ERROR("Attempting to update pipeline with invalid resources");
			return;
		}

		Ref<ResourceSetOpenGL> resourceSet = std::dynamic_pointer_cast<ResourceSetOpenGL>(desc.TargetResourceSet);
		m_BoundResourceSet				   = desc;
	}

	void CommandExecutorOpenGL::ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)
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

					float scissorY = m_CurrentRenderTarget->GetWidth() - m_CurrentRenderTarget->GetHeight() - rect.Y;
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

	void CommandExecutorOpenGL::ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)
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

	void CommandExecutorOpenGL::ExecuteCommand(WeakRef<IFramebuffer> command, IGraphicsDevice *device)
	{
		GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;

		if (Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(command.lock()))
		{
			GL::ExecuteGLCommands([&](const GladGLContext &context) { framebuffer->BindAsDrawBuffer(context); });
			m_CurrentRenderTarget = framebuffer;
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(const Viewport &command, IGraphicsDevice *device)
	{
		if (!ValidateForSetViewport(m_CurrentRenderTarget, command))
		{
			return;
		}

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				float left	 = command.X;
				float bottom = m_CurrentRenderTarget->GetHeight() - (command.Y + command.Height);

				glCall(context.Viewport(left, bottom, command.Width, command.Height));
				glCall(context.DepthRangef(command.MinDepth, command.MaxDepth));
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const Scissor &command, IGraphicsDevice *device)
	{
		if (!ValidateForSetScissor(m_CurrentRenderTarget, command))
		{
			return;
		}

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				float scissorY = m_CurrentRenderTarget->GetHeight() - command.Height - command.Y;

				glCall(context.Scissor(command.X, scissorY, command.Width, command.Height));
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)
	{
		if (!ValidateForResolve(command))
		{
			return;
		}

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				Point2D<uint32_t> size = Utils::GetMipSize(command.Source->GetWidth(), command.Source->GetHeight(), command.SourceMipLevel);

				Graphics::TextureCopyDescription copyDesc = {};
				copyDesc.Source							  = command.Source;
				copyDesc.Destination					  = command.Destination;
				copyDesc.SourceOffset					  = {0, 0, (int32_t)command.SourceArrayLayer};
				copyDesc.DestinationOffset				  = {0, 0, (int32_t)command.DestinationArrayLayer};
				copyDesc.SourceMipLevel					  = command.SourceMipLevel;
				copyDesc.DestinationMipLevel			  = command.DestinationMipLevel;
				copyDesc.Extent							  = {size.X, size.Y, 1};

				GL::CopyTextureToTexture(copyDesc, context);

				Ref<DeviceBufferOpenGL> buffer = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.Destination);
				buffer->MarkDirty();
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)
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
				uint64_t now   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				query->m_Start = now;
	#else
				GLint64 timer;
				glCall(context.GetInteger64v(GL_TIMESTAMP, &timer));
				query->m_Start = (uint64_t)timer;
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)
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
				uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				query->m_End = now;
	#else
				GLint64 timer;
				glCall(context.GetInteger64v(GL_TIMESTAMP, &timer));
				query->m_End = (uint64_t)timer;
	#endif
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferTextureCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.BufferTextureCopy.TextureHandle);

		GL::ExecuteGLCommands([&](const GladGLContext &context) { GL::CopyBufferToTexture(command, context); });

		Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(command.BufferTextureCopy.TextureHandle);
		texture->MarkDirty();
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> buffer		  = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.TextureBufferCopy.BufferHandle);
		Ref<TextureOpenGL>		textureOpenGL = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureBufferCopy.TextureHandle);

		GL::ExecuteGLCommands([&](const GladGLContext &context) { GL::CopyTextureToBuffer(command, context); });
	}

	void CommandExecutorOpenGL::ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)
	{
		Ref<TextureOpenGL>			  sourceTexture = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Source);
		Ref<TextureOpenGL>			  destTexture	= std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Destination);
		const TextureCopyDescription &copyDesc		= command.TextureCopy;

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_copy_image || context.VERSION_4_3)
				{
					context.CopyImageSubData(sourceTexture->GetHandle(),
											 sourceTexture->GetTextureType(),
											 copyDesc.SourceMipLevel,
											 copyDesc.SourceOffset.X,
											 copyDesc.SourceOffset.Y,
											 copyDesc.SourceOffset.Z,
											 destTexture->GetHandle(),
											 destTexture->GetTextureType(),
											 copyDesc.DestinationMipLevel,
											 copyDesc.DestinationOffset.X,
											 copyDesc.DestinationOffset.Y,
											 copyDesc.DestinationOffset.Z,
											 copyDesc.Extent.Width,
											 copyDesc.Extent.Height,
											 copyDesc.Extent.Depth);
				}
				else
				{
					GL::CopyTextureToTexture(copyDesc, context);
				}
			});

		Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(command.TextureCopy.Destination);
		texture->MarkDirty();
	}

	void CommandExecutorOpenGL::ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context)
							  { glCall(context.PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, command.GroupName.c_str())); });
	}

	void CommandExecutorOpenGL::ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.PopDebugGroup(); });
	}

	void CommandExecutorOpenGL::ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)
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

	void CommandExecutorOpenGL::ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{ context.BlendColor(command.BlendFactor.Red, command.BlendFactor.Green, command.BlendFactor.Blue, command.BlendFactor.Alpha); });
	}

	void CommandExecutorOpenGL::ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)
	{
		if (m_CurrentlyBoundPipeline.has_value())
		{
			Ref<Pipeline> pipeline = m_CurrentlyBoundPipeline.value();
			if (pipeline->GetType() == PipelineType::Graphics)
			{
				Ref<GraphicsPipelineOpenGL> pipelineGL = std::dynamic_pointer_cast<GraphicsPipelineOpenGL>(pipeline);
				GL::ExecuteGLCommands([&](const GladGLContext &context) { pipelineGL->SetStencilReference(context, command.StencilReference); });
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)
	{
		if (m_BoundResourceSet.has_value())
		{
			Ref<ResourceSetOpenGL> resourceSet = std::dynamic_pointer_cast<ResourceSetOpenGL>(m_BoundResourceSet.value().TargetResourceSet);
			resourceSet->SetPushConstants(command.Name, command.Data.data(), command.Offset, command.Data.size());
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(const MemoryBarrierDesc &command, IGraphicsDevice *device)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				// check that we have a version of OpenGL that supports memory barriers
				bool barrierSupported = context.ES_VERSION_3_1 || context.VERSION_4_2 || context.MemoryBarrierEXT;

				if (barrierSupported)
				{
					// convert into OpenGL barrier flags and whether the flags can be used with a region barrier
					bool supportsByRegion = false;

					// check if the OpenGL context version supports storage buffers
					bool	   supportsStorageBuffers = context.ES_VERSION_3_1 || context.VERSION_4_3;
					GLbitfield barrierFlags			  = GL::GetBarrierFlags(command.AfterAccess, supportsStorageBuffers, supportsByRegion);

					// we check if the OpenGL context supports memory barriers by region and this feature is supported with the requested barrier
					// access. This will make some barriers e.g. framebuffer reads after writes much more efficient.
					if (supportsByRegion && context.MemoryBarrierByRegion != nullptr)
					{
						context.MemoryBarrierByRegion(barrierFlags);
					}
					// otherwise, we have to use a global memory barrier if they are supported
					else if (context.MemoryBarrierEXT != nullptr)
					{
						context.MemoryBarrierEXT(barrierFlags);
					}
				}
			});
	}

	void CommandExecutorOpenGL::ExecuteCommand(const TextureBarrierDesc &command, IGraphicsDevice *device)
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.TextureBarrier(); });

		const SubresourceRange &range = command.TextureSubresourceRange;

		Ref<TextureOpenGL> textureGL = std::dynamic_pointer_cast<TextureOpenGL>(command.ITexture);

		for (uint32_t arrayLayer = range.BaseArrayLayer; arrayLayer < range.BaseArrayLayer + range.LayerCount; arrayLayer++)
		{
			for (uint32_t mipLevel = range.BaseMipLevel; mipLevel < range.BaseMipLevel + range.LevelCount; mipLevel++)
			{
				textureGL->SetTextureLayout(arrayLayer, mipLevel, command.Layout);
			}
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(const BufferBarrierDesc &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)
	{
	}

	void CommandExecutorOpenGL::BindResourceSet(const GladGLContext &context)
	{
		Nexus::Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		if (!pipeline)
			return;

		pipeline->Bind(context);

		if (m_BoundResourceSet.has_value())
		{
			ResourceSetBindingDescription bindingDescription = m_BoundResourceSet.value();
			Ref<ResourceSetOpenGL>		  resourceSet		 = std::dynamic_pointer_cast<ResourceSetOpenGL>(bindingDescription.TargetResourceSet);
			if (resourceSet)
			{
				resourceSet->Bind(bindingDescription, pipeline->GetShaderHandle(), context);
			}
		}
	}
}	 // namespace Nexus::Graphics

#endif