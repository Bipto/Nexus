#if defined(NX_PLATFORM_OPENGL)

#include "CommandExecutorOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"
#include "TimingQueryOpenGL.hpp"

namespace Nexus::Graphics
{
    CommandExecutorOpenGL::~CommandExecutorOpenGL()
    {
        Reset();
    }

    void CommandExecutorOpenGL::ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDeviceOpenGL *device)
    {
        for (const auto &element : commands)
        {
            std::visit(
                [&](auto &&arg)
                {
                    ExecuteCommand(arg, device);
                },
                element);
        }
    }

    void CommandExecutorOpenGL::Reset()
    {
        m_CurrentlyBoundPipeline = {};
        m_CurrentlyBoundVertexBuffers = {};
        m_CurrentRenderTarget = {};
    }

    void CommandExecutorOpenGL::ExecuteCommand(SetVertexBufferCommand command, GraphicsDeviceOpenGL *device)
    {
        auto vertexBufferGL = std::dynamic_pointer_cast<VertexBufferOpenGL>(command.VertexBufferRef);
        vertexBufferGL->Bind();
        m_CurrentlyBoundVertexBuffers[command.Slot] = vertexBufferGL;
    }

    void CommandExecutorOpenGL::ExecuteCommand(Ref<IndexBuffer> command, GraphicsDeviceOpenGL *device)
    {
        auto indexBufferGL = std::dynamic_pointer_cast<IndexBufferOpenGL>(command);
        indexBufferGL->Bind();
        m_IndexBufferFormat = GL::GetGLIndexBufferFormat(indexBufferGL->GetFormat());
    }

    void CommandExecutorOpenGL::ExecuteCommand(Ref<Pipeline> command, GraphicsDeviceOpenGL *device)
    {
        auto pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(command);
        auto target = pipeline->GetPipelineDescription().Target;

        ExecuteCommand(SetRenderTargetCommand{target}, device);

        // unbind the current pipeline before binding the new one
        if (Ref<PipelineOpenGL> oldPipeline = m_CurrentlyBoundPipeline.lock())
        {
            oldPipeline->Unbind();
        }

        // bind the pipeline
        pipeline->Bind();
        m_CurrentlyBoundPipeline = pipeline;
    }

    void CommandExecutorOpenGL::ExecuteCommand(DrawElementCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentlyBoundPipeline.expired())
        {
            NX_ERROR("Attempting to submit draw command without a bound pipeline");
            return;
        }

        if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.lock()))
        {
            pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, 0, 0);

            glCall(glDrawArrays(
                GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
                command.Start,
                command.Count));
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(DrawIndexedCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentlyBoundPipeline.expired())
        {
            NX_ERROR("Attempting to submit draw command without a bound pipeline");
            return;
        }

        if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.lock()))
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

            glCall(glDrawElements(
                GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
                command.Count,
                m_IndexBufferFormat,
                (void *)offset));
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(DrawInstancedCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentlyBoundPipeline.expired())
        {
            NX_ERROR("Attempting to submit draw command without a bound pipeline");
            return;
        }

        if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.lock()))
        {
            pipeline->BindVertexBuffers(m_CurrentlyBoundVertexBuffers, 0, command.InstanceStart);

            glCall(glDrawArraysInstanced(
                GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
                command.VertexStart,
                command.VertexCount,
                command.InstanceCount));
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentlyBoundPipeline.expired())
        {
            NX_ERROR("Attempting to submit draw command without a bound pipeline");
            return;
        }

        if (Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.lock()))
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

            glCall(glDrawElementsInstanced(
                GL::GetTopology(pipeline->GetPipelineDescription().PrimitiveTopology),
                command.IndexCount,
                m_IndexBufferFormat,
                (void *)offset,
                command.InstanceCount));
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(UpdateResourcesCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentlyBoundPipeline.expired())
        {
            NX_ERROR("Attempting to bind resources without a bound pipeline");
            return;
        }

        if (command.Resources.expired())
        {
            NX_ERROR("Attempting to update pipeline with invalid resources");
            return;
        }

        Nexus::Ref<PipelineOpenGL> pipeline = std::dynamic_pointer_cast<PipelineOpenGL>(m_CurrentlyBoundPipeline.lock());
        Nexus::Ref<ResourceSetOpenGL> resourceSet = std::dynamic_pointer_cast<ResourceSetOpenGL>(command.Resources.lock());

        const auto &textureBindings = resourceSet->GetBoundTextures();
        const auto &uniformBufferBindings = resourceSet->GetBoundUniformBuffers();
        const auto &samplerBindings = resourceSet->GetBoundSamplers();

        for (const auto &texture : textureBindings)
        {
            GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), texture.first.c_str());
            glCall(glUniform1i(location, location));
            glCall(glActiveTexture(GL_TEXTURE0 + location));
            glCall(glBindTexture(GL_TEXTURE_2D, texture.second->GetNativeHandle()));
        }

        for (const auto &sampler : samplerBindings)
        {
            GLint location = glGetUniformLocation(pipeline->GetShaderHandle(), sampler.first.c_str());
            glCall(glBindSampler(location, sampler.second->GetHandle()));
        }

        GLuint uniformBufferSlot = 0;
        for (const auto &uniformBuffer : uniformBufferBindings)
        {
            GLint location = glGetUniformBlockIndex(pipeline->GetShaderHandle(), uniformBuffer.first.c_str());

            glCall(glUniformBlockBinding(pipeline->GetShaderHandle(),
                                         location,
                                         uniformBufferSlot));

            glCall(glBindBufferRange(GL_UNIFORM_BUFFER,
                                     uniformBufferSlot,
                                     uniformBuffer.second->GetHandle(),
                                     0,
                                     uniformBuffer.second->GetDescription().Size));

            uniformBufferSlot++;
        }
    }

    void CommandExecutorOpenGL::ExecuteCommand(ClearColorTargetCommand command, GraphicsDeviceOpenGL *device)
    {
        float color[] =
            {
                command.Color.Red,
                command.Color.Green,
                command.Color.Blue,
                command.Color.Alpha};

        glCall(glClearBufferfv(
            GL_COLOR,
            command.Index,
            color));
    }

    void CommandExecutorOpenGL::ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDeviceOpenGL *device)
    {
        glCall(glClearBufferfi(GL_DEPTH_STENCIL, 0, command.Value.Depth, command.Value.Stencil));
    }

    void CommandExecutorOpenGL::ExecuteCommand(SetRenderTargetCommand command, GraphicsDeviceOpenGL *device)
    {
        // handle different options of render targets
        if (Nexus::Graphics::Swapchain **swapchain = command.Target.GetDataIf<Swapchain *>())
        {
            device->SetSwapchain(*swapchain);
        }

        if (Nexus::Ref<Nexus::Graphics::Framebuffer> *framebuffer = command.Target.GetDataIf<Nexus::Ref<Nexus::Graphics::Framebuffer>>())
        {
            device->SetFramebuffer(*framebuffer);
        }

        m_CurrentRenderTarget = command.Target;
    }

    void CommandExecutorOpenGL::ExecuteCommand(SetViewportCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentRenderTarget.GetType() == RenderTargetType::None)
        {
            NX_ERROR("Attempting to set a viewport without first setting a render target");
            return;
        }

        float left = command.Viewport.X;
        float bottom = m_CurrentRenderTarget.GetSize().Y - (command.Viewport.Y + command.Viewport.Height);

        glCall(glViewport(
            left,
            bottom,
            command.Viewport.Width,
            command.Viewport.Height));

        glCall(glDepthRangef(
            command.Viewport.MinDepth,
            command.Viewport.MaxDepth));
    }

    void CommandExecutorOpenGL::ExecuteCommand(SetScissorCommand command, GraphicsDeviceOpenGL *device)
    {
        if (m_CurrentRenderTarget.GetType() == RenderTargetType::None)
        {
            NX_ERROR("Attempting to set a viewport without first setting a render target");
            return;
        }

        float scissorY = m_CurrentRenderTarget.GetSize().Y - command.Scissor.Height - command.Scissor.Y;

        glCall(glScissor(
            command.Scissor.X,
            scissorY,
            command.Scissor.Width,
            command.Scissor.Height));
    }

    void CommandExecutorOpenGL::ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDeviceOpenGL *device)
    {
        if (command.Source.expired())
        {
            NX_ERROR("Attempting to resolve from an invalid framebuffer");
            return;
        }

        Ref<FramebufferOpenGL> framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(command.Source.lock());
        SwapchainOpenGL *swapchain = (SwapchainOpenGL *)command.Target;

        framebuffer->BindAsReadBuffer(command.SourceIndex);
        swapchain->BindAsDrawTarget();

        uint32_t framebufferWidth = framebuffer->GetFramebufferSpecification().Width;
        uint32_t framebufferHeight = framebuffer->GetFramebufferSpecification().Height;

        Nexus::Window *window = swapchain->GetWindow();
        uint32_t swapchainWidth = window->GetWindowSize().X;
        uint32_t swapchainHeight = window->GetWindowSize().Y;

        glCall(glBlitFramebuffer(
            0, 0,
            framebufferWidth, framebufferHeight,
            0, 0,
            swapchainWidth, swapchainHeight,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR));
    }

    void CommandExecutorOpenGL::ExecuteCommand(StartTimingQueryCommand command, GraphicsDeviceOpenGL *device)
    {
        if (command.Query.expired())
        {
            NX_ERROR("Attempting to write a timestamp to an invalid query object");
            return;
        }

        Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query.lock());

        glFinish();
        GLint64 timer;
        glCall(glGetInteger64v(GL_TIMESTAMP, &timer));
        query->m_Start = (uint64_t)timer;
    }

    void CommandExecutorOpenGL::ExecuteCommand(StopTimingQueryCommand command, GraphicsDeviceOpenGL *device)
    {
        if (command.Query.expired())
        {
            NX_ERROR("Attempting to write a timestamp to an invalid query object");
            return;
        }

        Ref<TimingQueryOpenGL> query = std::dynamic_pointer_cast<TimingQueryOpenGL>(command.Query.lock());

        glFinish();
        GLint64 timer;
        glCall(glGetInteger64v(GL_TIMESTAMP, &timer));
        query->m_End = (uint64_t)timer;
    }
}

#endif