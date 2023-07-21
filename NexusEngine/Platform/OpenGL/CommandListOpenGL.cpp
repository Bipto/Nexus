#include "CommandListOpenGL.hpp"

#include "PipelineOpenGL.hpp"
#include "ShaderOpenGL.hpp"
#include "BufferOpenGL.hpp"
#include "TextureOpenGL.hpp"

#include <memory>
#include <stdexcept>

namespace Nexus::Graphics
{
    void CommandListOpenGL::Begin(const CommandListBeginInfo &beginInfo)
    {
        m_CommandIndex = 0;
        m_VertexBufferIndex = 0;
        m_IndexBufferIndex = 0;
        m_PipelineIndex = 0;
        m_ElementCommandIndex = 0;
        m_IndexedCommandIndex = 0;
        m_TextureCommandIndex = 0;

        m_CommandDataIndex = 0;

        m_Pipelines.clear();
        m_ElementCommands.clear();
        m_IndexedCommands.clear();
        m_TextureUpdateCommands.clear();
        m_CommandData.clear();

        m_CommandListBeginInfo.ClearValue = beginInfo.ClearValue;
        m_CommandListBeginInfo.DepthValue = beginInfo.DepthValue;
        m_CommandListBeginInfo.StencilValue = beginInfo.StencilValue;

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto color = commandList->GetClearColorValue();

            glClearColor(color.Red,
                         color.Green,
                         color.Blue,
                         color.Alpha);
            glClearDepthf(commandList->GetClearDepthValue());
            glClearStencil(commandList->GetClearStencilValue());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        };
    }

    void CommandListOpenGL::End()
    {
    }

    void CommandListOpenGL::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        // m_VertexBuffers.push_back(vertexBuffer);
        m_CommandData.push_back(vertexBuffer.get());
        auto vb = (VertexBufferOpenGL *)vertexBuffer.get();

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto vertexBuffer = (VertexBufferOpenGL *)commandListGL->GetCurrentCommandData();
            vertexBuffer->Bind();
        };
    }

    void CommandListOpenGL::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        // m_IndexBuffers.push_back(indexBuffer);
        m_CommandData.push_back(indexBuffer.get());
        auto ib = (IndexBufferOpenGL *)indexBuffer.get();

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto indexBuffer = (IndexBufferOpenGL *)commandListGL->GetCurrentCommandData();
            indexBuffer->Bind();
        };
    }

    void CommandListOpenGL::SetPipeline(Ref<Pipeline> pipeline)
    {
        // m_Pipelines.push_back(pipeline);
        m_CommandData.push_back(pipeline.get());

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            // commandList->BindNextPipeline();

            auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto pipeline = (PipelineOpenGL *)commandListGL->GetCurrentCommandData();
            pipeline->Bind();
            commandListGL->SetCurrentPipeline(pipeline);
        };
    }

    void CommandListOpenGL::DrawElements(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_ElementCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListOpenGL> oglCL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto drawCommand = oglCL->GetCurrentDrawElementCommand();
            glDrawArrays(oglCL->GetTopology(), drawCommand.Start, drawCommand.Count);
        };
    }

    void CommandListOpenGL::DrawIndexed(uint32_t count, uint32_t offset)
    {
        DrawIndexedCommand command;
        command.Count = count;
        command.Offset = offset;
        m_IndexedCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListOpenGL> oglCL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
            auto drawCommand = oglCL->GetCurrentDrawIndexedCommand();
            auto topology = oglCL->GetTopology();
            glDrawElements(topology, drawCommand.Count, GL_UNSIGNED_INT, (void *)drawCommand.Offset);
        };
    }

    void CommandListOpenGL::UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding)
    {
        TextureUpdateCommand command;
        command.Texture = texture;
        command.Shader = shader;
        command.Binding = binding;
        m_TextureUpdateCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto textureCommand = commandList->GetCurrentTextureUpdateCommand();

            textureCommand.Shader->SetTexture(
                textureCommand.Texture,
                textureCommand.Binding);
        };
    }

    void CommandListOpenGL::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
        UniformBufferUpdateCommand command;
        command.Buffer = buffer;
        command.Data = new char[size];
        command.Size = size;
        command.Offset = offset;
        memcpy(command.Data, data, size);
        m_UniformBufferUpdateCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto uniformBufferCommand = commandList->GetCurrentUniformBufferUpdateCommand();
            uniformBufferCommand.Buffer->SetData(
                uniformBufferCommand.Data,
                uniformBufferCommand.Size,
                uniformBufferCommand.Offset);
        };
    }

    const ClearValue &CommandListOpenGL::GetClearColorValue()
    {
        return m_CommandListBeginInfo.ClearValue;
    }

    const float CommandListOpenGL::GetClearDepthValue()
    {
        return m_CommandListBeginInfo.DepthValue;
    }

    const uint8_t CommandListOpenGL::GetClearStencilValue()
    {
        return m_CommandListBeginInfo.StencilValue;
    }

    void CommandListOpenGL::BindNextPipeline()
    {
        auto pipeline = m_Pipelines[m_PipelineIndex++];
        Ref<PipelineOpenGL> pl = std::dynamic_pointer_cast<PipelineOpenGL>(pipeline);
        pl->Bind();
        m_CurrentPipeline = pipeline.get();
    }

    DrawElementCommand &CommandListOpenGL::GetCurrentDrawElementCommand()
    {
        return m_ElementCommands[m_ElementCommandIndex++];
    }

    DrawIndexedCommand &CommandListOpenGL::GetCurrentDrawIndexedCommand()
    {
        return m_IndexedCommands[m_IndexedCommandIndex++];
    }

    TextureUpdateCommand &CommandListOpenGL::GetCurrentTextureUpdateCommand()
    {
        return m_TextureUpdateCommands[m_TextureCommandIndex++];
    }

    UniformBufferUpdateCommand &CommandListOpenGL::GetCurrentUniformBufferUpdateCommand()
    {
        return m_UniformBufferUpdateCommands[m_UniformBufferUpdateCommandIndex++];
    }

    void *CommandListOpenGL::GetCurrentCommandData()
    {
        return m_CommandData[m_CommandDataIndex++];
    }

    GLenum CommandListOpenGL::GetTopology()
    {
        GLenum drawMode;
        auto topology = m_CurrentPipeline->GetPipelineDescription().PrimitiveTopology;

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

    Pipeline *CommandListOpenGL::GetCurrentPipeline()
    {
        return m_CurrentPipeline;
    }

    const std::array<RenderCommand, 1000> &CommandListOpenGL::GetRenderCommands()
    {
        return m_Commands;
    }

    uint32_t CommandListOpenGL::GetCommandCount()
    {
        return m_CommandIndex;
    }

    void CommandListOpenGL::SetCurrentPipeline(Pipeline *pipeline)
    {
        m_CurrentPipeline = pipeline;
    }
}