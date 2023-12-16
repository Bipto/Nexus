#include "CommandListD3D11.hpp"

#include "GraphicsDeviceD3D11.hpp"
#include "BufferD3D11.hpp"
#include "PipelineD3D11.hpp"
#include "ShaderD3D11.hpp"
#include "RenderPassD3D11.hpp"
#include "ResourceSetD3D11.hpp"
#include "TextureD3D11.hpp"

#include <memory>

#if defined(NX_PLATFORM_D3D11)
DXGI_FORMAT GetD3DIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
{
    switch (format)
    {
    case Nexus::Graphics::IndexBufferFormat::UInt16:
        return DXGI_FORMAT_R16_UINT;
    case Nexus::Graphics::IndexBufferFormat::UInt32:
        return DXGI_FORMAT_R32_UINT;
    }
}
#endif

namespace Nexus::Graphics
{
    CommandListD3D11::CommandListD3D11(GraphicsDeviceD3D11 *graphicsDevice)
    {
        m_GraphicsDevice = graphicsDevice;

        const uint32_t initialCommandCount = 100000;
        m_Commands.resize(initialCommandCount);
        m_CommandData.resize(initialCommandCount);
    }

    void CommandListD3D11::Begin()
    {
#if defined(NX_PLATFORM_D3D11)
        m_Commands.clear();
        m_CommandData.clear();
        m_CommandIndex = 0;
#endif
    }

    void CommandListD3D11::End()
    {
    }

    void CommandListD3D11::BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo)
    {
#if defined(NX_PLATFORM_D3D11)

        BeginRenderPassCommand command;
        command.ClearValue = beginInfo;
        command.RenderPass = renderPass;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            const auto &renderPassCommand = std::get<BeginRenderPassCommand>(commandData);
            const auto renderPass = renderPassCommand.RenderPass;
            const auto &beginInfo = renderPassCommand.ClearValue;

            // bind targets
            {
                if (renderPass->GetRenderPassDataType() == Nexus::Graphics::RenderPassDataType::Framebuffer)
                {
                    RenderPassD3D11 *renderPassD3D11 = (RenderPassD3D11 *)renderPass;
                    Framebuffer *framebuffer = renderPassD3D11->GetFramebuffer();
                    graphicsDevice->SetFramebuffer(framebuffer);
                }
                else
                {
                    Swapchain *swapchain = renderPass->GetData<Swapchain *>();
                    graphicsDevice->SetSwapchain(swapchain);
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
                    if (depthStencilView)
                    {
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
            }
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListD3D11::EndRenderPass()
    {
    }

    void CommandListD3D11::SetVertexBuffer(VertexBuffer *vertexBuffer)
    {
#if defined(NX_PLATFORM_D3D11)
        m_CommandData.emplace_back(vertexBuffer);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            const auto vertexBuffer = std::get<VertexBuffer *>(commandData);
            auto vertexBufferD3D11 = (VertexBufferD3D11 *)vertexBuffer;

            const auto &layout = vertexBufferD3D11->GetLayout();
            uint32_t stride = layout.GetStride();
            uint32_t offset = 0;

            auto nativeBuffer = vertexBufferD3D11->GetHandle();

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

    void CommandListD3D11::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
#if defined(NX_PLATFORM_D3D11)
        m_CommandData.emplace_back(indexBuffer);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();

            auto indexBuffer = std::get<IndexBuffer *>(commandData);
            auto context = graphicsDevice->GetDeviceContext();
            auto indexBufferD3D11 = (IndexBufferD3D11 *)indexBuffer;

            auto nativeBuffer = indexBufferD3D11->GetHandle();

            auto bufferFormat = indexBufferD3D11->GetFormat();

            context->IASetIndexBuffer(
                nativeBuffer,
                GetD3DIndexBufferFormat(indexBufferD3D11->GetFormat()),
                0);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListD3D11::SetPipeline(Pipeline *pipeline)
    {
#if defined(NX_PLATFORM_D3D11)
        m_CommandData.emplace_back(pipeline);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            auto pipeline = std::get<Pipeline *>(commandData);
            commandListD3D11->BindPipeline(pipeline);
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListD3D11::DrawElements(uint32_t start, uint32_t count)
    {
#if defined(NX_PLATFORM_D3D11)
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            const auto &drawCommand = std::get<DrawElementCommand>(commandData);

            context->Draw(drawCommand.Count, drawCommand.Start);
        };
        m_Commands.push_back(renderCommand);

#endif
    }

    void CommandListD3D11::DrawIndexed(uint32_t count, uint32_t offset)
    {
#if defined(NX_PLATFORM_D3D11)
        DrawIndexedCommand command;
        command.Count = count;
        command.Offset = offset;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            const auto &drawCommand = std::get<DrawIndexedCommand>(commandData);

            context->DrawIndexed(drawCommand.Count, drawCommand.Offset, 0);
        };
        m_Commands.push_back(renderCommand);

#endif
    }

    void CommandListD3D11::SetResourceSet(ResourceSet *resources)
    {
#if defined(NX_PLATFORM_D3D11)
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            auto commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDeviceD3D11 = (GraphicsDeviceD3D11 *)commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDeviceD3D11->GetDeviceContext();

            const auto &updateResourceCommand = std::get<UpdateResourcesCommand>(commandListD3D11->GetCurrentCommandData());
            auto resourceSetD3D11 = (ResourceSetD3D11 *)updateResourceCommand.Resources;

            // update textures
            for (const auto &textureBinding : resourceSetD3D11->GetTextureBindings())
            {
                auto textureD3D11 = (TextureD3D11 *)textureBinding.second;

                auto resourceView = textureD3D11->GetResourceView();
                auto samplerState = textureD3D11->GetSamplerState();

                context->PSSetShaderResources(textureBinding.first, 1, (ID3D11ShaderResourceView *const *)&resourceView);
                context->PSSetSamplers(textureBinding.first, 1, (ID3D11SamplerState *const *)&samplerState);
            }

            // update uniform buffers
            for (const auto &uniformBufferBinding : resourceSetD3D11->GetUniformBufferBindings())
            {
                auto uniformBufferD3D11 = (UniformBufferD3D11 *)uniformBufferBinding.second;
                auto bufferHandle = uniformBufferD3D11->GetHandle();

                context->VSSetConstantBuffers(
                    uniformBufferBinding.first,
                    1,
                    &bufferHandle);
            }
        };
        m_Commands.push_back(renderCommand);
#endif
    }

    void CommandListD3D11::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            auto clearColorCommand = std::get<ClearColorTargetCommand>(commandData);

            float backgroundColor[4] =
                {
                    clearColorCommand.Color.Red,
                    clearColorCommand.Color.Green,
                    clearColorCommand.Color.Blue,
                    clearColorCommand.Color.Alpha};

            auto activeRenderTargetViews = graphicsDevice->GetActiveRenderTargetViews();
            if (activeRenderTargetViews.size() > clearColorCommand.Index)
            {
                auto target = activeRenderTargetViews[clearColorCommand.Index];
                context->ClearRenderTargetView(target, backgroundColor);
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListD3D11::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            auto clearDepthCommand = std::get<ClearDepthStencilTargetCommand>(commandData);

            auto activeDepthStencilView = graphicsDevice->GetActiveDepthStencilView();
            if (activeDepthStencilView)
            {
                uint32_t clearFlags = 0;
                clearFlags |= D3D11_CLEAR_DEPTH;
                clearFlags |= D3D11_CLEAR_STENCIL;

                context->ClearDepthStencilView(activeDepthStencilView, clearFlags, clearDepthCommand.Value.Depth, clearDepthCommand.Value.Stencil);
            }
        };
        m_Commands.push_back(renderCommand);
    }

    void CommandListD3D11::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command{target};
        m_CommandData.emplace_back(command);

        auto renderCommand = [](CommandList *commandList)
        {
            CommandListD3D11 *commandListD3D11 = (CommandListD3D11 *)commandList;
            auto graphicsDevice = commandListD3D11->GetGraphicsDevice();
            auto context = graphicsDevice->GetDeviceContext();
            const auto &commandData = commandListD3D11->GetCurrentCommandData();
            auto setRenderTargetCommand = std::get<SetRenderTargetCommand>(commandData);

            if (setRenderTargetCommand.Target.GetType() == RenderTargetType::Swapchain)
            {
                Swapchain *swapchain = setRenderTargetCommand.Target.GetData<Swapchain *>();
                graphicsDevice->SetSwapchain(swapchain);
            }
            else if (setRenderTargetCommand.Target.GetType() == RenderTargetType::Framebuffer)
            {
                Framebuffer *framebuffer = setRenderTargetCommand.Target.GetData<Framebuffer *>();
                graphicsDevice->SetFramebuffer(framebuffer);
            }
            else
            {
                throw std::runtime_error("Invalid render target type entered");
            }
        };
        m_Commands.push_back(renderCommand);
    }

    const std::vector<RenderCommand> &CommandListD3D11::GetRenderCommands()
    {
        return m_Commands;
    }

    RenderCommandData &CommandListD3D11::GetCurrentCommandData()
    {
        return m_CommandData[m_CommandIndex++];
    }

    GraphicsDeviceD3D11 *CommandListD3D11::GetGraphicsDevice()
    {
        return m_GraphicsDevice;
    }

    Pipeline *CommandListD3D11::GetCurrentPipeline()
    {
        return m_CurrentPipeline;
    }

    void CommandListD3D11::BindPipeline(Pipeline *pipeline)
    {
#if defined(NX_PLATFORM_D3D11)
        auto pipelineD3D11 = (PipelineD3D11 *)pipeline;
        m_CurrentPipeline = pipeline;

        auto depthStencilState = pipelineD3D11->GetDepthStencilState();
        auto rasterizerState = pipelineD3D11->GetRasterizerState();
        auto blendState = pipelineD3D11->GetBlendState();
        const auto &viewport = pipelineD3D11->GetViewport();
        const auto &scissorRectangle = pipelineD3D11->GetScissorRectangle();
        auto topology = pipelineD3D11->GetTopology();
        auto shader = pipeline->GetShader();
        auto dxShader = (ShaderD3D11 *)shader;

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