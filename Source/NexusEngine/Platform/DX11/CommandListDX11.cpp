#include "CommandListDX11.hpp"

#include "GraphicsDeviceDX11.hpp"
#include "BufferDX11.hpp"
#include "PipelineDX11.hpp"
#include "ShaderDX11.hpp"
#include "RenderPassDX11.hpp"

#include <memory>

namespace Nexus::Graphics
{
    CommandListDX11::CommandListDX11(GraphicsDeviceDX11 *graphicsDevice)
    {
        m_GraphicsDevice = graphicsDevice;

        const uint32_t initialCommandCount = 100000;
        m_Commands.resize(initialCommandCount);
        m_CommandData.resize(initialCommandCount);
    }

    void CommandListDX11::Begin()
    {
#if defined(NX_PLATFORM_DX11)
        m_Commands.clear();
        m_CommandData.clear();
        m_CommandIndex = 0;
#endif
    }

    void CommandListDX11::End()
    {
    }

    void CommandListDX11::BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo)
    {
#if defined(NX_PLATFORM_DX11)

        BeginRenderPassCommand command;
        command.ClearValue = beginInfo;
        command.RenderPass = renderPass;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = commandListDX11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            const auto &renderPassCommand = std::get<BeginRenderPassCommand>(commandData);
            const auto renderPass = renderPassCommand.RenderPass;
            const auto &beginInfo = renderPassCommand.ClearValue;

            // bind framebuffer
            {
                if (renderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Framebuffer)
                {
                    auto renderPassDX11 = std::dynamic_pointer_cast<RenderPassDX11>(renderPass);
                    graphicsDevice->SetFramebuffer(renderPassDX11->m_Framebuffer);
                }
                else
                {
                    graphicsDevice->SetFramebuffer(nullptr);
                }
            }

            // clear targets
            {
                const auto &color = beginInfo.ClearColorValue;
                auto activeRenderTargetViews = graphicsDevice->GetActiveRenderTargetViews();

                float backgroundColor[4] = {
                    color.Red,
                    color.Green,
                    color.Blue,
                    color.Alpha};

                if (renderPass->GetColorLoadOperation() == Nexus::Graphics::LoadOperation::Clear)
                {
                    for (auto target : activeRenderTargetViews)
                        context->ClearRenderTargetView(target, backgroundColor);
                }

                if (renderPass->GetDepthStencilLoadOperation() == Nexus::Graphics::LoadOperation::Clear)
                {
                    auto depthStencilView = graphicsDevice->GetActiveDepthStencilView();
                    uint32_t clearFlags = 0;
                    clearFlags |= D3D11_CLEAR_DEPTH;
                    clearFlags |= D3D11_CLEAR_STENCIL;

                    context->ClearDepthStencilView(
                        depthStencilView,
                        clearFlags,
                        beginInfo.ClearDepthStencilValue.Depth,
                        beginInfo.ClearDepthStencilValue.Stencil);
                }
            }
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListDX11::EndRenderPass()
    {
    }

    void CommandListDX11::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
#if defined(NX_PLATFORM_DX11)
        m_CommandData.emplace_back(vertexBuffer);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = commandListDX11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            const auto vertexBuffer = std::get<Ref<VertexBuffer>>(commandData);
            auto vertexBufferDX11 = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);

            const auto &layout = vertexBufferDX11->GetLayout();
            uint32_t stride = layout.GetStride();
            uint32_t offset = 0;

            auto nativeBuffer = vertexBufferDX11->GetHandle();

            context->IASetVertexBuffers(
                0,
                1,
                &nativeBuffer,
                &stride,
                &offset);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListDX11::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
#if defined(NX_PLATFORM_DX11)
        m_CommandData.emplace_back(indexBuffer);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = commandListDX11->GetGraphicsDevice();
            const auto &commandData = commandListDX11->GetCurrentCommandData();

            auto indexBuffer = std::get<Ref<IndexBuffer>>(commandData);
            auto context = graphicsDevice->GetDeviceContext();
            auto indexBufferDX11 = std::dynamic_pointer_cast<IndexBufferDX11>(indexBuffer);

            auto nativeBuffer = indexBufferDX11->GetHandle();

            context->IASetIndexBuffer(
                nativeBuffer,
                DXGI_FORMAT_R32_UINT,
                0);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListDX11::SetPipeline(Ref<Pipeline> pipeline)
    {
#if defined(NX_PLATFORM_DX11)
        m_CommandData.emplace_back(pipeline);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            Ref<CommandListDX11> commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            auto pipeline = std::get<Ref<Pipeline>>(commandData);
            commandListDX11->BindPipeline(pipeline);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListDX11::DrawElements(uint32_t start, uint32_t count)
    {
#if defined(NX_PLATFORM_DX11)
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = commandListDX11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            const auto &drawCommand = std::get<DrawElementCommand>(commandData);

            context->Draw(drawCommand.Count, drawCommand.Start);
        };
        m_Commands.push_back(renderCommand);

#endif
    }

    void CommandListDX11::DrawIndexed(uint32_t count, uint32_t offset)
    {
#if defined(NX_PLATFORM_DX11)
        DrawIndexedCommand command;
        command.Count = count;
        command.Offset = offset;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            auto graphicsDevice = commandListDX11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            const auto &drawCommand = std::get<DrawIndexedCommand>(commandData);

            context->DrawIndexed(drawCommand.Count, drawCommand.Offset, 0);
        };
        m_Commands.push_back(renderCommand);

#endif
    }

    void CommandListDX11::UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureResourceBinding &binding)
    {
#if defined(NX_PLATFORM_DX11)

        TextureUpdateCommand command;
        command.Texture = texture;
        command.Shader = shader;
        command.Binding = binding;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            auto &textureUpdateCommand = std::get<TextureUpdateCommand>(commandData);

            textureUpdateCommand.Shader->SetTexture(
                textureUpdateCommand.Texture,
                textureUpdateCommand.Binding);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListDX11::UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset)
    {
#if defined(NX_PLATFORM_DX11)
        UniformBufferUpdateCommand command;
        command.Buffer = buffer;
        command.Data = new char[size];
        command.Size = size;
        command.Offset = offset;
        memcpy(command.Data, data, size);
        m_CommandData.emplace_back(command);

        auto renderCommand = [](Ref<CommandList> commandList)
        {
            auto commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
            const auto &commandData = commandListDX11->GetCurrentCommandData();
            const auto &uniformBufferUpdateCommand = std::get<UniformBufferUpdateCommand>(commandData);

            uniformBufferUpdateCommand.Buffer->SetData(
                uniformBufferUpdateCommand.Data,
                uniformBufferUpdateCommand.Size,
                uniformBufferUpdateCommand.Offset);

            delete uniformBufferUpdateCommand.Data;
        };
        m_Commands.push_back(renderCommand);

#endif
    }

    void CommandListDX11::WriteTexture(Ref<Texture> texture, Ref<ResourceSet> resourceSet, uint32_t binding)
    {
    }

    void CommandListDX11::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, Ref<ResourceSet> resourceSet, uint32_t binding)
    {
    }

    void CommandListDX11::SetResources(Ref<ResourceSet> resourceSet)
    {
    }

    const std::vector<RenderCommand> &CommandListDX11::GetRenderCommands()
    {
        return m_Commands;
    }

    RenderCommandData &CommandListDX11::GetCurrentCommandData()
    {
        return m_CommandData[m_CommandIndex++];
    }

    GraphicsDeviceDX11 *CommandListDX11::GetGraphicsDevice()
    {
        return m_GraphicsDevice;
    }

    Ref<Pipeline> CommandListDX11::GetCurrentPipeline()
    {
        return m_CurrentPipeline;
    }

    void CommandListDX11::BindPipeline(Ref<Pipeline> pipeline)
    {
#if defined(NX_PLATFORM_DX11)
        auto pipelineDX11 = std::dynamic_pointer_cast<PipelineDX11>(pipeline);
        m_CurrentPipeline = pipeline;

        auto depthStencilState = pipelineDX11->GetDepthStencilState();
        auto rasterizerState = pipelineDX11->GetRasterizerState();
        auto blendState = pipelineDX11->GetBlendState();
        const auto &viewport = pipelineDX11->GetViewport();
        const auto &scissorRectangle = pipelineDX11->GetScissorRectangle();
        auto topology = pipelineDX11->GetTopology();
        auto shader = pipeline->GetShader();
        auto dxShader = std::dynamic_pointer_cast<ShaderDX11>(shader);

        auto context = m_GraphicsDevice->GetDeviceContext();

        context->OMSetDepthStencilState(depthStencilState, 0);
        context->RSSetState(rasterizerState);
        context->RSSetViewports(1, &viewport);
        context->RSSetScissorRects(1, &scissorRectangle);

        context->OMSetBlendState(blendState, NULL, 0xffffffff);

        context->IASetPrimitiveTopology(topology);
        context->VSSetShader(dxShader->GetVertexShader(), 0, 0);
        context->PSSetShader(dxShader->GetPixelShader(), 0, 0);
        context->IASetInputLayout(dxShader->GetInputLayout());
#endif
    }
}