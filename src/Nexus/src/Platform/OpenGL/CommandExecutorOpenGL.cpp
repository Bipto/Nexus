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

		if (Ref<VertexBuffer> vb = command.VertexBufferRef.lock())
		{
			Ref<VertexBufferOpenGL> vertexBufferGL = std::dynamic_pointer_cast<VertexBufferOpenGL>(vb);
			vertexBufferGL->Bind();
			m_CurrentlyBoundVertexBuffers[command.Slot] = vertexBufferGL;
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}
		if (Ref<IndexBuffer> ib = command.lock())
		{
			auto indexBufferGL = std::dynamic_pointer_cast<IndexBufferOpenGL>(ib);
			indexBufferGL->Bind();
			m_IndexBufferFormat = GL::GetGLIndexBufferFormat(indexBufferGL->GetFormat());
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)
	{
		if (command.expired())
		{
			NX_ERROR("Attempting to bind an invalid pipeline");
			return;
		}

		if (Ref<Pipeline> pl = command.lock())
		{
			auto pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(pl);

			// unbind the current pipeline before binding the new one
			if (m_CurrentlyBoundPipeline.has_value())
			{
				Ref<PipelineOpenGL> oldPipeline = m_CurrentlyBoundPipeline.value();
				oldPipeline->Unbind();
			}

			// bind the pipeline
			pipeline->Bind();
			m_CurrentlyBoundPipeline = pipeline;
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawElementCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value()))
		{
			pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, 0, 0);

			glCall(glDrawArrays(GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology), command.Start, command.Count));
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value()))
		{
			pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, command.VertexStart, 0);

			uint32_t indexSize = 0;
			if (m_IndexBufferFormat == GL_UNSIGNED_SHORT)
			{
				indexSize = sizeof(uint16_t);
			}
			else if (m_IndexBufferFormat == GL_UNSIGNED_INT)
			{
				indexSize = sizeof(uint32_t);
			}
			else
			{
				NX_ERROR("Undefined index buffer format supplied");
				return;
			}

			uint32_t offset = command.IndexStart * indexSize;

			glCall(glDrawElements(GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
								  command.Count,
								  m_IndexBufferFormat,
								  (void *)offset));
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value()))
		{
			pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, 0, command.InstanceStart);

			glCall(glDrawArraysInstanced(GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
										 command.VertexStart,
										 command.VertexCount,
										 command.InstanceCount));
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value()))
		{
			pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, command.VertexStart, command.InstanceStart);

			uint32_t indexSize = 0;
			if (m_IndexBufferFormat == GL_UNSIGNED_SHORT)
			{
				indexSize = sizeof(uint16_t);
			}
			else if (m_IndexBufferFormat == GL_UNSIGNED_INT)
			{
				indexSize = sizeof(uint32_t);
			}
			else
			{
				NX_ERROR("Undefined index buffer format supplied");
				return;
			}

			uint32_t offset = command.IndexStart * indexSize;

			glCall(glDrawElementsInstanced(GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
										   command.IndexCount,
										   m_IndexBufferFormat,
										   (void *)offset,
										   command.InstanceCount));
		}
	}

	void CommandExecutorOpenGL::ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)
	{
		if (!ValidateForGraphicsCall(m_CurrentlyBoundPipeline, m_CurrentRenderTarget))
		{
			return;
		}

		if (!command)
		{
			NX_ERROR("Attempting to update pipeline with invalid resources");
			return;
		}

		Nexus::Ref<PipelineOpenGL>	  pipeline	  = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.value());
		Nexus::Ref<ResourceSetOpenGL> resourceSet = std::dynamic_pointer_cast<ResourceSetOpenGL>(command);

		const auto &combinedImageSamplers = resourceSet->GetBoundCombinedImageSamplers();
		const auto &uniformBufferBindings = resourceSet->GetBoundUniformBuffers();

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

			// bind texture
			{
				GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());
			}

			// find the slot in the shader where the uniform is located
			GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), name.c_str());

			// bind texture2D if needed
			if (std::holds_alternative<WeakRef<Texture2D>>(combinedImageSampler.ImageTexture))
			{
				WeakRef<Texture2D> texture = std::get<WeakRef<Texture2D>>(combinedImageSampler.ImageTexture);
				if (Ref<Texture2DOpenGL> glTexture = std::dynamic_pointer_cast<Texture2DOpenGL>(texture.lock()))
				{
					glTexture->Bind(location);
					glSampler->Bind(location);
				}
			}

			// bind cubemap if needed
			else if (std::holds_alternative<WeakRef<Cubemap>>(combinedImageSampler.ImageTexture))
			{
				WeakRef<Cubemap> cubemap = std::get<WeakRef<Cubemap>>(combinedImageSampler.ImageTexture);
				if (Ref<CubemapOpenGL> glCubemap = std::dynamic_pointer_cast<CubemapOpenGL>(cubemap.lock()))
				{
					glCubemap->Bind(location);
					glSampler->Bind(location);
				}
			}

			else
			{
				throw std::runtime_error("Attempting to bind invalid texture type");
			}
		}

		GLuint uniformBufferSlot = 0;
		for (const auto [name, uniformBuffer] : uniformBufferBindings)
		{
			if (uniformBuffer.expired())
			{
				NX_ERROR("Attempting to bind an invalid uniform buffer");
				continue;
			}

			if (Ref<UniformBufferOpenGL> uniformBufferGL = std::dynamic_pointer_cast<UniformBufferOpenGL>(uniformBuffer.lock()))
			{
				GLint location = glGetUniformBlockIndex(pipeline->GetShaderHandle(), name.c_str());

				glCall(glUniformBlockBinding(pipeline->GetShaderHandle(), location, uniformBufferSlot));

				glCall(
				glBindBufferRange(GL_UNIFORM_BUFFER, uniformBufferSlot, uniformBufferGL->GetHandle(), 0, uniformBufferGL->GetDescription().Size));

				uniformBufferSlot++;
			}
		}
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

		glCall(glClearBufferfi(GL_DEPTH_STENCIL, 0, command.Value.Depth, command.Value.Stencil));
	}

	void CommandExecutorOpenGL::ExecuteCommand(RenderTarget command, GraphicsDevice *device)
	{
		GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;

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

		Nexus::Window *window		   = swapchain->GetWindow();
		uint32_t	   swapchainWidth  = window->GetWindowSize().X;
		uint32_t	   swapchainHeight = window->GetWindowSize().Y;

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

	#if defined(__EMSCRIPTEN__)
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

	#if defined(__EMSCRIPTEN__)
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
}	 // namespace Nexus::Graphics

#endif