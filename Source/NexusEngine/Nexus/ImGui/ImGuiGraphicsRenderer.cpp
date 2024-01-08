#include "ImGuiGraphicsRenderer.hpp"

namespace Nexus::ImGuiUtils
{
    ImGuiGraphicsRenderer::ImGuiGraphicsRenderer(Nexus::Application *app)
    {
        m_GraphicsDevice = app->GetGraphicsDevice();

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        Nexus::Graphics::VertexBufferLayout layout =
            {
                {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}};

        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("resources/shaders/imgui.glsl", layout);

        Nexus::Graphics::PipelineDescription pipelineDesc;
        pipelineDesc.Shader = m_Shader;
        pipelineDesc.Target = {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()};
        pipelineDesc.BlendStateDescription.EnableBlending = true;

        pipelineDesc.BlendStateDescription.SourceColourBlend = Nexus::Graphics::BlendFunction::SourceAlpha;
        pipelineDesc.BlendStateDescription.DestinationColourBlend = Nexus::Graphics::BlendFunction::OneMinusSourceAlpha;
        pipelineDesc.BlendStateDescription.BlendEquation = Nexus::Graphics::BlendEquation::Add;
        pipelineDesc.BlendStateDescription.SourceAlphaBlend = Nexus::Graphics::BlendFunction::One;
        pipelineDesc.BlendStateDescription.DestinationAlphaBlend = Nexus::Graphics::BlendFunction::OneMinusSourceAlpha;

        pipelineDesc.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
        pipelineDesc.RasterizerStateDescription.FillMode = Nexus::Graphics::FillMode::Solid;

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(glm::mat4);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

        Nexus::Graphics::TextureResourceBinding textureBinding;
        textureBinding.Slot = 0;
        textureBinding.Name = "Texture";

        Nexus::Graphics::UniformResourceBinding uniformBinding;
        uniformBinding.Binding = 0;
        uniformBinding.Name = "MVP";
        uniformBinding.Buffer = m_UniformBuffer;

        Nexus::Graphics::ResourceSetSpecification resources;
        resources.TextureBindings = {textureBinding};
        resources.UniformResourceBindings = {uniformBinding};
        pipelineDesc.ResourceSetSpecification = resources;

        m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDesc);

        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);

        auto context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);

        auto &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    }

    void ImGuiGraphicsRenderer::RebuildFontAtlas()
    {
        auto &io = ImGui::GetIO();
        unsigned char *pixels;
        int width, height, channels;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &channels);

        Nexus::Graphics::TextureSpecification spec;
        spec.Width = width;
        spec.Height = height;
        spec.NumberOfChannels = channels;
        spec.Format = Nexus::Graphics::TextureFormat::RGBA8;
        spec.SamplerState = Nexus::Graphics::SamplerState::LinearClamp;

        m_FontTexture = m_GraphicsDevice->CreateTexture(spec);
        m_FontTexture->SetData(pixels, width * height * channels * sizeof(unsigned char));

        UnbindTexture(m_FontTextureID);

        m_FontTextureID = BindTexture(m_FontTexture);
        io.Fonts->SetTexID(m_FontTextureID);
        io.Fonts->ClearTexData();
    }

    ImTextureID ImGuiGraphicsRenderer::BindTexture(Nexus::Graphics::Texture *texture)
    {
        auto id = (ImTextureID)m_TextureID++;
        m_BoundTextures.insert({id, texture});
        return id;
    }

    void ImGuiGraphicsRenderer::UnbindTexture(ImTextureID id)
    {
        m_BoundTextures.erase(id);
    }

    void ImGuiGraphicsRenderer::BeforeLayout(Nexus::Time gameTime)
    {
        auto &io = ImGui::GetIO();
        io.DeltaTime = (float)gameTime.GetSeconds();

        auto windowSize = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();
        io.DisplaySize = {(float)windowSize.X, (float)windowSize.Y};
        io.DisplayFramebufferScale = {1.0f, 1.0f};

        UpdateInput();
        ImGui::NewFrame();
    }

    void ImGuiGraphicsRenderer::AfterLayout()
    {
        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiGraphicsRenderer::UpdateInput()
    {
    }

    void ImGuiGraphicsRenderer::RenderDrawData(ImDrawData *drawData)
    {
        drawData->ScaleClipRects(ImGui::GetIO().DisplayFramebufferScale);
        UpdateBuffers(drawData);
        RenderCommandLists(drawData);
    }

    void ImGuiGraphicsRenderer::UpdateBuffers(ImDrawData *drawData)
    {
        if (drawData->TotalVtxCount == 0)
        {
            return;
        }

        if (drawData->TotalVtxCount > m_VertexBufferCount)
        {
            delete m_VertexBuffer;

            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}};

            Nexus::Graphics::BufferDescription desc;
            desc.Size = drawData->TotalVtxCount * 1.5f * sizeof(ImDrawVert);
            desc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(desc, nullptr, layout);
        }

        if (drawData->TotalIdxCount > m_IndexBufferCount)
        {
            delete m_IndexBuffer;

            Nexus::Graphics::BufferDescription desc;
            desc.Size = drawData->TotalIdxCount * 1.5f * sizeof(ImDrawIdx);
            desc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(desc, nullptr, Nexus::Graphics::IndexBufferFormat::UInt16);
        }

        ImDrawVert *vtxDst = (ImDrawVert *)m_VertexBuffer->Map();
        ImDrawIdx *idxDst = (ImDrawIdx *)m_IndexBuffer->Map();

        for (int i = 0; i < drawData->CmdListsCount; i++)
        {
            const ImDrawList *cmdList = drawData->CmdLists[i];
            memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.size_in_bytes());
            memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.size_in_bytes());

            vtxDst += cmdList->VtxBuffer.size_in_bytes();
            idxDst += cmdList->IdxBuffer.size_in_bytes();
        }

        m_VertexBuffer->Unmap();
        m_IndexBuffer->Unmap();
    }

    void ImGuiGraphicsRenderer::RenderCommandLists(ImDrawData *drawData)
    {
        if (drawData->TotalVtxCount == 0)
            return;

        m_CommandList->Begin();

        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->SetVertexBuffer(m_VertexBuffer);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);

        auto &io = ImGui::GetIO();
        glm::mat4 mvp = glm::ortho<float>(0, io.DisplaySize.x, io.DisplaySize.y, 0, -1.f, 1.0f);
        void *buffer = m_UniformBuffer->Map();
        memcpy(buffer, &mvp, sizeof(mvp));
        m_UniformBuffer->Unmap();

        m_ResourceSet->WriteTexture(m_FontTexture, 0);
        m_ResourceSet->WriteUniformBuffer(m_UniformBuffer, 0);
        m_CommandList->SetResourceSet(m_ResourceSet);

        auto windowSize = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();
        Nexus::Graphics::Viewport viewport;
        viewport.X = 0;
        viewport.Y = 0;
        viewport.Width = windowSize.X;
        viewport.Height = windowSize.Y;
        m_CommandList->SetViewport(viewport);

        int vtxOffset = 0;
        int idxOffset = 0;

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            ImDrawList *cmdList = drawData->CmdLists[n];

            for (int cmdi = 0; cmdi < cmdList->CmdBuffer.Size; cmdi++)
            {
                ImDrawCmd drawCmd = cmdList->CmdBuffer[cmdi];

                if (drawCmd.ElemCount > 0)
                {
                    Nexus::Graphics::Scissor scissor;
                    scissor.X = drawCmd.ClipRect.x;
                    scissor.Y = drawCmd.ClipRect.y;
                    scissor.Width = (uint32_t)(drawCmd.ClipRect.z - drawCmd.ClipRect.x);
                    scissor.Height = (uint32_t)(drawCmd.ClipRect.w - drawCmd.ClipRect.y);
                    m_CommandList->SetScissor(scissor);

                    m_CommandList->DrawIndexed(
                        drawCmd.ElemCount,
                        drawCmd.IdxOffset,
                        drawCmd.VtxOffset);
                }
            }

            idxOffset += cmdList->IdxBuffer.Size;
            vtxOffset += cmdList->VtxBuffer.Size;
        }

        m_CommandList->End();

        m_GraphicsDevice->SubmitCommandList(m_CommandList);
    }
}