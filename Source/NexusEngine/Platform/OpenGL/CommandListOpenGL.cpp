#include "CommandListOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "RenderPassOpenGL.hpp"

#include <memory>
#include <stdexcept>

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
    }

    void CommandListOpenGL::End()
    {
    }

    void CommandListOpenGL::BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo)
    {
        BeginRenderPassCommand command;
        command.ClearValue = beginInfo;
        command.RenderPass = renderPass;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &renderPassCommand = std::get<BeginRenderPassCommand>(commandData);
            const auto renderPass = renderPassCommand.RenderPass;
            const auto &beginInfo = renderPassCommand.ClearValue;
            auto graphicsDevice = (GraphicsDeviceOpenGL *)commandListGL->GetGraphicsDevice();

            // bind framebuffers
            {
                if (renderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Framebuffer)
                {
                    auto renderPassOpenGL = std::dynamic_pointer_cast<RenderPassOpenGL>(renderPass);
                    graphicsDevice->SetFramebuffer(renderPassOpenGL->m_Framebuffer);
                }
                else
                {
                    graphicsDevice->SetFramebuffer(nullptr);
                }
            }

            // clear targets
            {
                const auto &color = beginInfo.ClearColorValue;

                glClearColor(color.Red,
                             color.Green,
                             color.Blue,
                             color.Alpha);
                glClearDepthf(beginInfo.ClearDepthStencilValue.Depth);
                glClearStencil(beginInfo.ClearDepthStencilValue.Stencil);

                uint32_t clearFlags = 0;

                if (renderPass->GetColorLoadOperation() == Nexus::Graphics::LoadOperation::Clear)
                {
                    clearFlags |= GL_COLOR_BUFFER_BIT;
                }

                if (renderPass->GetDepthStencilLoadOperation() == Nexus::Graphics::LoadOperation::Clear)
                {
                    clearFlags |= GL_DEPTH_BUFFER_BIT;
                    clearFlags |= GL_STENCIL_BUFFER_BIT;
                }

                glClear(clearFlags);
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::EndRenderPass()
    {
    }

    void CommandListOpenGL::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        m_CommandData.emplace_back(vertexBuffer);
        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto vertexBuffer = std::get<Ref<VertexBuffer>>(commandData);
            const auto vertexBufferGL = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
            vertexBufferGL->Bind();
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
            commandListGL->BindPipeline(pipeline);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::DrawElements(uint32_t start, uint32_t count)
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
            glDrawArrays(commandListGL->GetTopology(), drawElementsCommand.Start, drawElementsCommand.Count);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::DrawIndexed(uint32_t count, uint32_t offset)
    {
        DrawIndexedCommand command;
        command.Count = count;
        command.Offset = offset;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &drawIndexedCommand = std::get<DrawIndexedCommand>(commandData);
            glDrawElements(commandListGL->GetTopology(), drawIndexedCommand.Count, GL_UNSIGNED_INT, (void *)drawIndexedCommand.Offset);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding)
    {
        TextureUpdateCommand command;
        command.Texture = texture;
        command.Shader = shader;
        command.Binding = binding;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &updateTextureCommand = std::get<TextureUpdateCommand>(commandData);

            updateTextureCommand.Shader->SetTexture(
                updateTextureCommand.Texture,
                updateTextureCommand.Binding);
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListOpenGL::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
        UniformBufferUpdateCommand command;
        command.Buffer = buffer;
        command.Data = new char[size];
        command.Size = size;
        command.Offset = offset;
        memcpy(command.Data, data, size);
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            const auto &commandData = commandListGL->GetCurrentCommandData();
            const auto &uniformBufferUpdateCommand = std::get<UniformBufferUpdateCommand>(commandData);

            void *buffer = uniformBufferUpdateCommand.Buffer->Map(MapMode::Write);
            memcpy(buffer, uniformBufferUpdateCommand.Data, uniformBufferUpdateCommand.Size);
            uniformBufferUpdateCommand.Buffer->Unmap();
            delete uniformBufferUpdateCommand.Data;
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
        auto pipelineGL = std::dynamic_pointer_cast<PipelineOpenGL>(pipeline);
        pipelineGL->Bind();
        m_CurrentlyBoundPipeline = pipeline;
    }

    GraphicsDevice *CommandListOpenGL::GetGraphicsDevice()
    {
        return m_Device;
    }
}