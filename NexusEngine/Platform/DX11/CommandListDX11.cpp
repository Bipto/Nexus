#include "CommandListDX11.h"

#include "GraphicsDeviceDX11.h"
#include "BufferDX11.h"

#include <memory>

namespace Nexus
{
    CommandListDX11::CommandListDX11(GraphicsDeviceDX11 *graphicsDevice, Ref<Pipeline> pipeline)
        : CommandList(pipeline)
    {
        m_GraphicsDevice = graphicsDevice;
    }

    void CommandListDX11::Begin(const CommandListBeginInfo &beginInfo)
    {
#if defined(NX_PLATFORM_DX11)
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
            Ref<CommandListDX11> dxCommandList = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = dxCommandList->GetGraphicsDevice();
            auto activeRenderTargetViews = graphicsDevice->GetActiveRenderTargetViews();
            auto context = graphicsDevice->GetDeviceContext();
            auto color = commandList->GetClearColorValue();

            for (auto target : activeRenderTargetViews)
            {

                float backgroundColor[4] = {
                    color.Red,
                    color.Green,
                    color.Blue,
                    color.Alpha};

                context->ClearRenderTargetView(target, backgroundColor);
            }

            auto depthStencilView = graphicsDevice->GetActiveDepthStencilView();
            if (depthStencilView)
            {
                context->ClearDepthStencilView(
                    depthStencilView,
                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                    commandList->GetClearDepthValue(),
                    commandList->GetClearStencilValue());
            }
        };
#endif
    }

    void CommandListDX11::End()
    {
    }

    void CommandListDX11::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
#if defined(NX_PLATFORM_DX11)

        m_VertexBuffers.push_back(vertexBuffer);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> dxCommandList = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = dxCommandList->GetGraphicsDevice();
            auto vertexBuffer = commandList->GetCurrentVertexBuffer();
            auto pipeline = commandList->GetPipeline();
            auto shader = pipeline->GetShader();
            auto layout = shader->GetLayout();
            auto context = graphicsDevice->GetDeviceContext();

            uint32_t stride = layout.GetStride();
            uint32_t offset = 0;

            Ref<VertexBufferDX11> vb = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);
            ID3D11Buffer *dx11VertexBuffer = vb->GetNativeHandle();

            context->IASetVertexBuffers(
                0,
                1,
                &dx11VertexBuffer,
                &stride,
                &offset);
        };
#endif
    }

    void CommandListDX11::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
#if defined(NX_PLATFORM_DX11)

        m_IndexBuffers.push_back(indexBuffer);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> dxCommandList = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = dxCommandList->GetGraphicsDevice();
            auto indexBuffer = commandList->GetCurrentIndexBuffer();
            auto context = graphicsDevice->GetDeviceContext();

            Ref<IndexBufferDX11> ib = std::dynamic_pointer_cast<IndexBufferDX11>(indexBuffer);
            ID3D11Buffer *dx11IndexBuffer = ib->GetNativeHandle();

            context->IASetIndexBuffer(
                dx11IndexBuffer,
                DXGI_FORMAT_R32_UINT,
                0);
        };
#endif
    }
    void CommandListDX11::DrawElements(uint32_t start, uint32_t count)
    {
#if defined(NX_PLATFORM_DX11)

        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_ElementCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> dxCommandList = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = dxCommandList->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            auto drawCommand = dxCommandList->GetCurrentDrawElementCommand();

            context->Draw(drawCommand.Count, drawCommand.Start);
        };
#endif
    }

    void CommandListDX11::DrawIndexed(uint32_t count, uint32_t offset)
    {
#if defined(NX_PLATFORM_DX11)

        DrawIndexedCommand command;
        command.Count = count;
        command.Offset = offset;
        m_IndexedCommands.push_back(command);

        m_Commands[m_CommandIndex++] = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> dxCommandList = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = dxCommandList->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            auto drawCommand = dxCommandList->GetCurrentDrawIndexedCommand();

            context->DrawIndexed(drawCommand.Count, drawCommand.Offset, 0);
        };
#endif
    }

    void CommandListDX11::UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding)
    {
#if defined(NX_PLATFORM_DX11)

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
#endif
    }

    void CommandListDX11::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
#if defined(NX_PLATFORM_DX11)

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
#endif
    }

    const ClearValue &CommandListDX11::GetClearColorValue()
    {
        return m_CommandListBeginInfo.ClearValue;
    }

    const float CommandListDX11::GetClearDepthValue()
    {
        return m_CommandListBeginInfo.DepthValue;
    }

    const uint8_t CommandListDX11::GetClearStencilValue()
    {
        return m_CommandListBeginInfo.StencilValue;
    }

    Ref<VertexBuffer> CommandListDX11::GetCurrentVertexBuffer()
    {
        return m_VertexBuffers[m_VertexBufferIndex++];
    }

    Ref<IndexBuffer> CommandListDX11::GetCurrentIndexBuffer()
    {
        return m_IndexBuffers[m_IndexBufferIndex++];
    }

    DrawElementCommand &CommandListDX11::GetCurrentDrawElementCommand()
    {
        return m_ElementCommands[m_ElementCommandIndex++];
    }

    DrawIndexedCommand &CommandListDX11::GetCurrentDrawIndexedCommand()
    {
        return m_IndexedCommands[m_IndexedCommandIndex++];
    }

    TextureUpdateCommand &CommandListDX11::GetCurrentTextureUpdateCommand()
    {
        return m_TextureUpdateCommands[m_TextureCommandIndex++];
    }

    UniformBufferUpdateCommand &CommandListDX11::GetCurrentUniformBufferCommand()
    {
        return m_UniformBufferUpdateCommands[m_UniformBufferUpdateCommandIndex++];
    }

    const std::array<RenderCommand, 1000> &CommandListDX11::GetRenderCommands()
    {
        return m_Commands;
    }

    uint32_t CommandListDX11::GetCommandCount()
    {
        return m_CommandIndex;
    }
}