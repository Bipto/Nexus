#if defined(NX_PLATFORM_OPENGL)

#include "CommandListOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "ResourceSetOpenGL.hpp"

#include "GL.hpp"

#include <memory>
#include <stdexcept>

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

    void CommandListOpenGL::Begin()
    {
        m_CommandIndex = 0;
        m_Commands.clear();
        m_CommandData.clear();

        m_CurrentlyBoundVertexBuffer = nullptr;
    }

    void CommandListOpenGL::End()
    {
    }

    void CommandListOpenGL::SetVertexBuffer(VertexBuffer *vertexBuffer)
    {
        m_CommandData.emplace_back(vertexBuffer);
        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto vertexBuffer = std::get<VertexBuffer *>(commandData);
            const auto vertexBufferGL = (VertexBufferOpenGL *)vertexBuffer;
            vertexBufferGL->Bind();
            commandListGL->m_CurrentlyBoundVertexBuffer = vertexBufferGL;
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
        m_CommandData.emplace_back(indexBuffer);
        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto indexBuffer = std::get<IndexBuffer *>(commandData);
            const auto indexBufferGL = (IndexBufferOpenGL *)indexBuffer;
            indexBufferGL->Bind();
            commandListGL->m_IndexBufferFormat = GetGLIndexBufferFormat(indexBufferGL->GetFormat());
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetPipeline(Pipeline *pipeline)
    {
        m_CommandData.emplace_back(pipeline);
        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto pipeline = std::get<Pipeline *>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            auto target = pipeline->GetPipelineDescription().Target;
            if (target.GetType() == RenderTargetType::Swapchain)
            {
                auto swapchain = target.GetData<Swapchain *>();
                graphicsDevice->SetSwapchain(swapchain);
            }
            else if (target.GetType() == RenderTargetType::Framebuffer)
            {
                auto framebuffer = target.GetData<Framebuffer *>();
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

    void CommandListOpenGL::DrawElements(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawElementsCommand = std::get<DrawElementCommand>(commandData);
            glDrawArrays(commandListGL->GetTopology(), drawElementsCommand.Start, drawElementsCommand.Count);
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

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawIndexedCommand = std::get<DrawIndexedCommand>(commandData);

            auto vertexBuffer = commandListGL->m_CurrentlyBoundVertexBuffer;
            vertexBuffer->SetupVertexArray(drawIndexedCommand.VertexStart);

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
            glDrawElements(commandListGL->GetTopology(), drawIndexedCommand.Count, commandListGL->m_IndexBufferFormat, (void *)offset);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetResourceSet(ResourceSet *resources)
    {
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            auto pipeline = commandListGL->m_CurrentlyBoundPipeline;
            const auto &updateResourcesCommand = std::get<UpdateResourcesCommand>(commandListGL->GetCurrentCommandData());

            auto shaderGL = (ShaderOpenGL *)pipeline->GetShader();
            auto resourcesGL = (ResourceSetOpenGL *)updateResourcesCommand.Resources;

            // update textures
            for (const auto &textureBinding : resourcesGL->GetTextureBindings())
            {
                auto textureGL = (TextureOpenGL *)textureBinding.second.Texture;

                auto location = glGetUniformLocation(shaderGL->GetHandle(), textureBinding.second.BindingName.c_str());
                glUniform1i(location, textureBinding.first);
                glActiveTexture(GL_TEXTURE0 + textureBinding.first);
                glBindTexture(GL_TEXTURE_2D, (unsigned int)textureGL->GetHandle());
            }

            // update uniform buffers
            for (const auto &uniformBufferBinding : resourcesGL->GetUniformBufferBindings())
            {
                auto uniformBufferGL = (UniformBufferOpenGL *)uniformBufferBinding.second.Buffer;
                unsigned int index = glGetUniformBlockIndex(shaderGL->GetHandle(), uniformBufferBinding.second.BindingName.c_str());
                glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferGL->GetHandle());
                glUniformBlockBinding(shaderGL->GetHandle(), index, uniformBufferBinding.first);
                glBindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBinding.first, uniformBufferGL->GetHandle(), 0, uniformBufferGL->GetDescription().Size);
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

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            auto commandData = commandListGL->GetCurrentCommandData();
            auto clearColorCommand = std::get<ClearColorTargetCommand>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            float color[] = {clearColorCommand.Color.Red,
                             clearColorCommand.Color.Green,
                             clearColorCommand.Color.Blue,
                             clearColorCommand.Color.Alpha};
            glClearBufferfv(GL_COLOR, 0, color);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            auto commandData = commandListGL->GetCurrentCommandData();
            auto clearDepthCommand = std::get<ClearDepthStencilTargetCommand>(commandData);
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            glClearBufferfi(GL_DEPTH_STENCIL, 0, clearDepthCommand.Value.Depth, clearDepthCommand.Value.Stencil);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command{target};
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
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
                auto framebuffer = setRenderTargetCommand.Target.GetData<Framebuffer *>();
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
        command.Viewport = viewport;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            auto commandData = commandListGL->GetCurrentCommandData();
            auto setViewportCommand = std::get<SetViewportCommand>(commandData);

            glViewport(
                setViewportCommand.Viewport.X,
                setViewportCommand.Viewport.Y,
                setViewportCommand.Viewport.Width,
                setViewportCommand.Viewport.Height);

            glDepthRangef(
                setViewportCommand.Viewport.MinDepth,
                setViewportCommand.Viewport.MaxDepth);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::SetScissor(const Scissor &scissor)
    {
        SetScissorCommand command;
        command.Scissor = scissor;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListGL = (CommandListOpenGL *)commandList;
            auto commandData = commandListGL->GetCurrentCommandData();
            auto setScissorCommand = std::get<SetScissorCommand>(commandData);

            auto scissorMinY = commandListGL->m_CurrentRenderTarget.GetSize().Y - setScissorCommand.Scissor.Height - setScissorCommand.Scissor.Y;

            glScissor(
                setScissorCommand.Scissor.X,
                scissorMinY,
                setScissorCommand.Scissor.Width,
                setScissorCommand.Scissor.Height);
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

    void CommandListOpenGL::BindPipeline(Pipeline *pipeline)
    {
        auto pipelineGL = (PipelineOpenGL *)pipeline;
        pipelineGL->Bind();
        m_CurrentlyBoundPipeline = pipeline;
    }

    GraphicsDevice *CommandListOpenGL::GetGraphicsDevice()
    {
        return m_Device;
    }
}

#endif