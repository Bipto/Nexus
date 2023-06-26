#include "CommandListOpenGL.h"

#include "PipelineOpenGL.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"

#include <memory>
#include <stdexcept>

namespace Nexus
{
    CommandListOpenGL::CommandListOpenGL(Ref<Pipeline> pipeline)
        : CommandList(pipeline)
    {
    }

    void CommandListOpenGL::Begin(const CommandListBeginInfo &beginInfo)
    {
        m_CommandIndex = 0;
        m_VertexBufferIndex = 0;
        m_IndexBufferIndex = 0;
        m_ElementCommandIndex = 0;
        m_IndexedCommandIndex = 0;
        m_TextureCommandIndex = 0;

        m_VertexBuffers.clear();
        m_IndexBuffers.clear();
        m_ElementCommands.clear();
        m_IndexedCommands.clear();
        m_TextureUpdateCommands.clear();

        m_CommandListBeginInfo.ClearValue = beginInfo.ClearValue;
        m_CommandListBeginInfo.DepthValue = beginInfo.DepthValue;

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
        m_VertexBuffers.push_back(vertexBuffer);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto vertexBuffer = commandList->GetCurrentVertexBuffer();
            auto openglVB = std::dynamic_pointer_cast<VertexBufferOpenGL>(vertexBuffer);
            openglVB->Bind();
        };
    }

    void CommandListOpenGL::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_IndexBuffers.push_back(indexBuffer);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto indexBuffer = commandList->GetCurrentIndexBuffer();
            auto openglIB = std::dynamic_pointer_cast<IndexBufferOpenGL>(indexBuffer);
            openglIB->Bind();
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
        command.texture = texture;
        command.shader = shader;
        command.binding = binding;
        m_TextureUpdateCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto textureCommand = commandList->GetCurrentTextureUpdateCommand();

            textureCommand.shader->SetTexture(
                textureCommand.texture,
                textureCommand.binding);
        };
    }

    void CommandListOpenGL::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
        UniformBufferUpdateCommand command;
        command.buffer = buffer;
        command.data = new char[size];
        command.size = size;
        command.offset = offset;
        memcpy(command.data, data, size);
        m_UniformBufferUpdateCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            auto uniformBufferCommand = commandList->GetCurrentUniformBufferCommand();
            uniformBufferCommand.buffer->SetData(
                uniformBufferCommand.data,
                uniformBufferCommand.size,
                uniformBufferCommand.offset);
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

    Ref<VertexBuffer> CommandListOpenGL::GetCurrentVertexBuffer()
    {
        auto vb = m_VertexBuffers[m_VertexBufferIndex++];
        return vb;
    }

    Ref<IndexBuffer> CommandListOpenGL::GetCurrentIndexBuffer()
    {
        return m_IndexBuffers[m_IndexBufferIndex++];
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

    UniformBufferUpdateCommand &CommandListOpenGL::GetCurrentUniformBufferCommand()
    {
        return m_UniformBufferUpdateCommands[m_UniformBufferUpdateCommandIndex++];
    }

    GLenum CommandListOpenGL::GetTopology()
    {
        GLenum drawMode;
        auto topology = GetPipeline()->GetPipelineDescription().PrimitiveTopology;

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

    const std::array<RenderCommand, 1000> &CommandListOpenGL::GetRenderCommands()
    {
        return m_Commands;
    }

    uint32_t CommandListOpenGL::GetCommandCount()
    {
        return m_CommandIndex;
    }
}