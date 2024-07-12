#if defined(NX_PLATFORM_OPENGL)

#include "CommandListOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"

#include "GL.hpp"

#include "Nexus/nxpch.hpp"

GLenum GetGLIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::IndexBufferFormat::UInt16:
        return GL_UNSIGNED_SHORT;
    case Nexus::Graphics::IndexBufferFormat::UInt32:
        return GL_UNSIGNED_INT;
    default:
        throw std::runtime_error("Failed to find a valid index buffer format");
    }
}

namespace Nexus::Graphics
{
    CommandListOpenGL::CommandListOpenGL(GraphicsDevice *device)
        : m_Device(device)
    {
        const uint32_t initialCommandCount = 100000;
        m_Commands.resize(initialCommandCount);
        m_CommandData.resize(initialCommandCount);
    }

    CommandListOpenGL::~CommandListOpenGL()
    {
        UnbindCurrentPipeline();
    }

    void CommandListOpenGL::Begin()
    {
        UnbindCurrentPipeline();

        m_CommandIndex = 0;
        m_Commands.clear();
        m_CommandData.clear();
        m_CurrentlyBoundVertexBuffers = {};
        m_CurrentlyBoundPipeline = nullptr;
    }

    void CommandListOpenGL::End()
    {
    }

    void CommandListOpenGL::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        SetVertexBufferCommand command;
        command.VertexBufferRef = vertexBuffer;
        command.Slot = slot;
        m_CommandData.emplace_back(command);
        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &setVertexBufferCommand = std::get<SetVertexBufferCommand>(commandData);
            const auto vertexBufferGL = std::dynamic_pointer_cast<VertexBufferOpenGL>(setVertexBufferCommand.VertexBufferRef);
            vertexBufferGL->Bind();
            commandListGL->m_CurrentlyBoundVertexBuffers[setVertexBufferCommand.Slot] = vertexBufferGL;
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_CommandData.emplace_back(indexBuffer);
        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto indexBuffer = std::get<Ref<IndexBuffer>>(commandData);
            const auto indexBufferGL = std::dynamic_pointer_cast<IndexBufferOpenGL>(indexBuffer);
            indexBufferGL->Bind();
            commandListGL->m_IndexBufferFormat = GetGLIndexBufferFormat(indexBufferGL->GetFormat());
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetPipeline(Ref<Pipeline> pipeline)
    {
        m_CommandData.emplace_back(pipeline);
        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto pipeline = std::get<Ref<Pipeline>>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            auto target = pipeline->GetPipelineDescription().Target;
            if (target.GetType() == RenderTargetType::Swapchain)
            {
                auto swapchain = target.GetData<Swapchain *>();
                graphicsDevice->SetSwapchain(swapchain);
            }
            else if (target.GetType() == RenderTargetType::Framebuffer)
            {
                auto framebuffer = target.GetData<Ref<Framebuffer>>();
                graphicsDevice->SetFramebuffer(framebuffer);
            }
            else
            {
                throw std::runtime_error("Invalid render target type");
            }

            commandListGL->BindPipeline(pipeline);
            commandListGL->m_CurrentRenderTarget = target;
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::Draw(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawElementsCommand = std::get<DrawElementCommand>(commandData);

            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;

            // we are only able to bind a vertex buffer if we know the layout of it
            if (pipeline)
            {
                for (const auto &vertexBuffer : commandListGL->m_CurrentlyBoundVertexBuffers)
                {
                    pipeline->BindVertexBuffers(commandListGL->m_CurrentlyBoundVertexBuffers, 0, 0);
                }
                glCall(glDrawArrays(commandListGL->GetTopology(),
                                    drawElementsCommand.Start,
                                    drawElementsCommand.Count));
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        DrawIndexedCommand command;
        command.Count = count;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawIndexedCommand = std::get<DrawIndexedCommand>(commandData);
            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;

            // we are only able to bind a vertex buffer if we know the layout of it
            if (pipeline)
            {
                pipeline->BindVertexBuffers(commandListGL->m_CurrentlyBoundVertexBuffers, drawIndexedCommand.VertexStart, 0);

                uint32_t indexSize = 0;
                if (commandListGL->m_IndexBufferFormat == GL_UNSIGNED_SHORT)
                {
                    indexSize = sizeof(uint16_t);
                }
                else
                {
                    indexSize = sizeof(uint32_t);
                }

                uint32_t offset = drawIndexedCommand.IndexStart * indexSize;
                glCall(glDrawElements(commandListGL->GetTopology(),
                                      drawIndexedCommand.Count,
                                      commandListGL->m_IndexBufferFormat,
                                      (void *)offset));
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        DrawInstancedCommand command;
        command.VertexCount = vertexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.InstanceStart = instanceStart;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawInstancedCommand = std::get<DrawInstancedCommand>(commandData);
            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;

            // we are only able to bind a vertex buffer if we know the layout of it
            if (pipeline)
            {
                pipeline->BindVertexBuffers(commandListGL->m_CurrentlyBoundVertexBuffers, 0, drawInstancedCommand.InstanceStart);

                uint32_t indexSize = 0;
                if (commandListGL->m_IndexBufferFormat == GL_UNSIGNED_SHORT)
                {
                    indexSize = sizeof(uint16_t);
                }
                else
                {
                    indexSize = sizeof(uint32_t);
                }

                glCall(glDrawArraysInstanced(commandListGL->GetTopology(),
                                             drawInstancedCommand.VertexStart,
                                             drawInstancedCommand.VertexCount,
                                             drawInstancedCommand.InstanceCount));
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        DrawInstancedIndexedCommand command;
        command.IndexCount = indexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.InstanceStart = instanceStart;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawIndexedInstanceCommand = std::get<DrawInstancedIndexedCommand>(commandData);
            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;

            // we are only able to bind a vertex buffer if we know the layout of it
            if (pipeline)
            {
                pipeline->BindVertexBuffers(commandListGL->m_CurrentlyBoundVertexBuffers, drawIndexedInstanceCommand.VertexStart, drawIndexedInstanceCommand.InstanceStart);

                uint32_t indexSize = 0;
                if (commandListGL->m_IndexBufferFormat == GL_UNSIGNED_SHORT)
                {
                    indexSize = sizeof(uint16_t);
                }
                else
                {
                    indexSize = sizeof(uint32_t);
                }

                uint32_t offset = drawIndexedInstanceCommand.IndexStart * indexSize;
                glCall(glDrawElementsInstanced(commandListGL->GetTopology(),
                                               drawIndexedInstanceCommand.IndexCount,
                                               commandListGL->m_IndexBufferFormat,
                                               (void *)offset,
                                               drawIndexedInstanceCommand.InstanceCount));
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetResourceSet(Ref<ResourceSet> resources)
    {
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;
            const auto &updateResourcesCommand = std::get<UpdateResourcesCommand>(commandListGL->GetCurrentCommandData());
            auto resourcesGL = std::dynamic_pointer_cast<ResourceSetOpenGL>(updateResourcesCommand.Resources);

            // upload resources
            const auto &textureBindings = resourcesGL->GetBoundTextures();
            const auto &uniformBufferBindings = resourcesGL->GetBoundUniformBuffers();
            const auto &samplerBindings = resourcesGL->GetBoundSamplers();

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

            GLint uniformBufferSlot = 0;
            for (const auto &uniformBuffer : uniformBufferBindings)
            {
                GLint location = glGetUniformBlockIndex(pipeline->GetShaderHandle(), uniformBuffer.first.c_str());
                glCall(glUniformBlockBinding(pipeline->GetShaderHandle(), location, uniformBufferSlot));

                glCall(glBindBufferRange(GL_UNIFORM_BUFFER,
                                         uniformBufferSlot,
                                         uniformBuffer.second->GetHandle(),
                                         0,
                                         uniformBuffer.second->GetDescription().Size));
                uniformBufferSlot++;
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto clearColorCommand = std::get<ClearColorTargetCommand>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            float color[] = {clearColorCommand.Color.Red,
                             clearColorCommand.Color.Green,
                             clearColorCommand.Color.Blue,
                             clearColorCommand.Color.Alpha};
            glCall(glClearBufferfv(GL_COLOR, 0, color));
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto clearDepthCommand = std::get<ClearDepthStencilTargetCommand>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            glCall(glClearBufferfi(GL_DEPTH_STENCIL, 0, clearDepthCommand.Value.Depth, clearDepthCommand.Value.Stencil));
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command{target};
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto setRenderTargetCommand = std::get<SetRenderTargetCommand>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            if (setRenderTargetCommand.Target.GetType() == RenderTargetType::Swapchain)
            {
                auto swapchain = setRenderTargetCommand.Target.GetData<Swapchain *>();
                graphicsDevice->SetSwapchain(swapchain);
            }
            else if (setRenderTargetCommand.Target.GetType() == RenderTargetType::Framebuffer)
            {
                auto framebuffer = setRenderTargetCommand.Target.GetData<Ref<Framebuffer>>();
                graphicsDevice->SetFramebuffer(framebuffer);
            }
            else
            {
                throw std::runtime_error("Invalid render target type");
            }

            commandListGL->m_CurrentRenderTarget = setRenderTargetCommand.Target;
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetViewport(const Viewport &viewport)
    {
        SetViewportCommand command;
        command.NextViewport = viewport;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto setViewportCommand = std::get<SetViewportCommand>(commandData);

            float left = setViewportCommand.NextViewport.X;
            float bottom = commandListGL->m_CurrentRenderTarget.GetSize().Y - (setViewportCommand.NextViewport.Y + setViewportCommand.NextViewport.Height);

            glCall(glViewport(
                left,
                bottom,
                setViewportCommand.NextViewport.Width,
                setViewportCommand.NextViewport.Height));

            glCall(glDepthRangef(
                setViewportCommand.NextViewport.MinDepth,
                setViewportCommand.NextViewport.MaxDepth));
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetScissor(const Scissor &scissor)
    {
        SetScissorCommand command;
        command.NextScissor = scissor;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto setScissorCommand = std::get<SetScissorCommand>(commandData);

            auto scissorMinY = commandListGL->m_CurrentRenderTarget.GetSize().Y - setScissorCommand.NextScissor.Height - setScissorCommand.NextScissor.Y;

            glCall(glScissor(
                setScissorCommand.NextScissor.X,
                scissorMinY,
                setScissorCommand.NextScissor.Width,
                setScissorCommand.NextScissor.Height));
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        ResolveSamplesToSwapchainCommand command;
        command.Source = source;
        command.SourceIndex = sourceIndex;
        command.Target = target;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto commandData = commandListGL->GetCurrentCommandData();
            auto resolveToSwapchainCommand = std::get<ResolveSamplesToSwapchainCommand>(commandData);

            Ref<FramebufferOpenGL> framebufferGL = std::dynamic_pointer_cast<FramebufferOpenGL>(resolveToSwapchainCommand.Source);
            SwapchainOpenGL *swapchainGL = (SwapchainOpenGL *)resolveToSwapchainCommand.Target;

            framebufferGL->BindAsReadBuffer(resolveToSwapchainCommand.SourceIndex);
            swapchainGL->BindAsDrawTarget();

            uint32_t framebufferWidth = framebufferGL->GetFramebufferSpecification().Width;
            uint32_t framebufferHeight = framebufferGL->GetFramebufferSpecification().Height;

            auto window = swapchainGL->GetWindow();
            uint32_t swapchainWidth = window->GetWindowSize().X;
            uint32_t swapchainHeight = window->GetWindowSize().Y;

            glCall(glBlitFramebuffer(0, 0,
                                     swapchainWidth, swapchainHeight,
                                     0, 0,
                                     swapchainWidth, swapchainHeight,
                                     GL_COLOR_BUFFER_BIT, GL_LINEAR));
        };
        m_Commands.push_back(renderCommand);
    }

    const std::vector<RenderCommand> &CommandListOpenGL::GetRenderCommands()
    {
        return m_Commands;
    }

    RenderCommandData &CommandListOpenGL::GetCurrentCommandData()
    {
        return m_CommandData[m_CommandIndex++];
    }

    GLenum CommandListOpenGL::GetTopology()
    {
        GLenum drawMode;
        auto topology = m_CurrentlyBoundPipeline->GetPipelineDescription().PrimitiveTopology;

        switch (topology)
        {
        case Topology::LineList:
            drawMode = GL_LINE_LOOP;
            break;
        case Topology::LineStrip:
            drawMode = GL_LINE_STRIP;
            break;
        case Topology::PointList:
            drawMode = GL_POINTS;
            break;
        case Topology::TriangleList:
            drawMode = GL_TRIANGLES;
            break;
        case Topology::TriangleStrip:
            drawMode = GL_TRIANGLE_STRIP;
            break;
        default:
            throw std::runtime_error("Invalid topology selected");
        }

        return drawMode;
    }

    void CommandListOpenGL::BindPipeline(Ref<Pipeline> pipeline)
    {
        UnbindCurrentPipeline();
        auto pipelineGL = std::dynamic_pointer_cast<PipelineOpenGL>(pipeline);
        pipelineGL->Bind();
        m_CurrentlyBoundPipeline = pipelineGL;
    }

    GraphicsDevice *CommandListOpenGL::GetGraphicsDevice()
    {
        return m_Device;
    }

    void CommandListOpenGL::UnbindCurrentPipeline()
    {
        if (m_CurrentlyBoundPipeline)
        {
            m_CurrentlyBoundPipeline->Unbind();
        }
    }
}

#endif