#include "ImGuiGraphicsRenderer.hpp"

#include "Nexus/Input/Input.hpp"

std::string GetImGuiShaderVertexSource()
{
    std::string shader =
        "#version 450 core\n"

        "layout(location = 0) in vec2 Position;\n"
        "layout(location = 1) in vec2 TexCoord;\n"
        "layout(location = 2) in vec4 Color;\n"

        "layout(location = 0) out vec2 Frag_UV;\n"
        "layout(location = 1) out vec4 Frag_Color;\n"

        "layout(binding = 0, set = 0) uniform MVP\n"
        "{\n"
        "    mat4 u_MVP;\n"
        "};\n"

        "void main()\n"
        "{\n"
        "    gl_Position = u_MVP * vec4(Position, 0.0, 1.0);\n"
        "    Frag_UV = vec2(TexCoord.x, TexCoord.y);\n"
        "    Frag_Color = Color;\n"
        "}";
    return shader;
}

std::string GetImGuiShaderFragmentSource()
{
    std::string shader =
        "#version 450 core\n"

        "layout(location = 0) in vec2 Frag_UV;\n"
        "layout(location = 1) in vec4 Frag_Color;\n"

        "layout(set = 1, binding = 0) uniform sampler2D Texture;\n"

        "layout(location = 0) out vec4 OutColor;\n"

        "void main()\n"
        "{\n"
        "    OutColor = Frag_Color * texture(Texture, Frag_UV.st)\n;"
        "}";
    return shader;
}

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

        auto vertexSource = GetImGuiShaderVertexSource();
        auto fragmentSource = GetImGuiShaderFragmentSource();
        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvSources(vertexSource, fragmentSource, layout, "imgui.vert", "imgui.frag");

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
        pipelineDesc.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(glm::mat4);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

        Nexus::Graphics::ResourceBinding textureBinding;
        textureBinding.Name = "Texture";
        textureBinding.Set = 1;
        textureBinding.Binding = 0;
        textureBinding.Type = Nexus::Graphics::ResourceType::CombinedImageSampler;

        Nexus::Graphics::ResourceBinding uniformBufferBinding;
        uniformBufferBinding.Name = "MVP";
        uniformBufferBinding.Set = 0;
        uniformBufferBinding.Binding = 0;
        uniformBufferBinding.Type = Nexus::Graphics::ResourceType::UniformBuffer;

        pipelineDesc.ResourceSetSpecification.Textures = {textureBinding};
        pipelineDesc.ResourceSetSpecification.UniformBuffers = {uniformBufferBinding};

        m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDesc);

        Nexus::Graphics::SamplerSpecification samplerSpec;
        samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Wrap;
        samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Wrap;
        samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Wrap;
        samplerSpec.MinimumLOD = 0;
        samplerSpec.MaximumLOD = 0;
        samplerSpec.LODBias = 0;
        samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
        m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

        auto context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);

        auto &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        // io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        // io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        io.Fonts->AddFontDefault();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        SetupInput();

        // look into implementing viewports
        auto &platformIo = ImGui::GetPlatformIO();

        // platform
        platformIo.Platform_CreateWindow = [](ImGuiViewport *vp) -> void {

        };

        platformIo.Platform_DestroyWindow = [](ImGuiViewport *vp) -> void {

        };

        platformIo.Platform_ShowWindow = [](ImGuiViewport *vp) -> void {

        };

        platformIo.Platform_SetWindowPos = [](ImGuiViewport *vp, ImVec2 pos) -> void {

        };

        platformIo.Platform_SetWindowSize = [](ImGuiViewport *vp, ImVec2 size) -> void {

        };

        platformIo.Platform_SetWindowFocus = [](ImGuiViewport *vp) -> void {

        };

        platformIo.Platform_GetWindowFocus = [](ImGuiViewport *vp) -> bool
        {
            return false;
        };

        platformIo.Platform_GetWindowMinimized = [](ImGuiViewport *vp) -> bool
        {
            return false;
        };

        platformIo.Platform_SetWindowTitle = [](ImGuiViewport *vp, const char *str) -> void {

        };

        platformIo.Platform_GetWindowDpiScale = [](ImGuiViewport *vp) -> float
        {
            return 0;
        };

        platformIo.Platform_UpdateWindow = [](ImGuiViewport *vp) -> void {

        };

        platformIo.Platform_GetWindowPos = [](ImGuiViewport *vp) -> ImVec2
        {
            return {};
        };

        platformIo.Platform_GetWindowSize = [](ImGuiViewport *vp) -> ImVec2
        {
            return {};
        };

        // rendering
        platformIo.Platform_RenderWindow = [](ImGuiViewport *vp, void *render_arg) -> void {

        };

        platformIo.Platform_SwapBuffers = [](ImGuiViewport *vp, void *render_arg) -> void {

        };
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
        spec.Format = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;

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

        auto resourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
        resourceSet->WriteCombinedImageSampler(texture, m_Sampler, "Texture");

        m_ResourceSets.insert({id, resourceSet});

        return id;
    }

    void ImGuiGraphicsRenderer::UnbindTexture(ImTextureID id)
    {
        m_ResourceSets.erase(id);
    }

    void ImGuiGraphicsRenderer::BeforeLayout(Nexus::Time gameTime)
    {
        auto &io = ImGui::GetIO();
        io.DeltaTime = (float)gameTime.GetSeconds();

        auto windowSize = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();
        io.DisplaySize = {(float)windowSize.X, (float)windowSize.Y};
        io.DisplayFramebufferScale = {1, 1};

        float scale = m_GraphicsDevice->GetPrimaryWindow()->GetDisplayScale();
        io.FontGlobalScale = scale;

        UpdateInput();
        ImGui::NewFrame();
    }

    void ImGuiGraphicsRenderer::AfterLayout()
    {
        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());
        UpdateCursor();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiGraphicsRenderer::SetupInput()
    {
        auto &io = ImGui::GetIO();

        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Tab] = (int)KeyCode::Tab);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_LeftArrow] = (int)KeyCode::KeyLeft);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_RightArrow] = (int)KeyCode::KeyRight);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_UpArrow] = (int)KeyCode::KeyUp);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_DownArrow] = (int)KeyCode::KeyDown);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_PageUp] = (int)KeyCode::PageUp);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_PageDown] = (int)KeyCode::PageDown);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Home] = (int)KeyCode::Home);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_End] = (int)KeyCode::End);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Delete] = (int)KeyCode::Delete);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Backspace] = (int)KeyCode::Back);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Enter] = (int)KeyCode::Enter);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Escape] = (int)KeyCode::Escape);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Space] = (int)KeyCode::Space);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_A] = (int)KeyCode::A);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_C] = (int)KeyCode::C);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_V] = (int)KeyCode::V);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_X] = (int)KeyCode::X);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Y] = (int)KeyCode::Y);
        m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Z] = (int)KeyCode::Z);

        auto input = m_GraphicsDevice->GetPrimaryWindow()->GetInput();
        input->TextInput += [&](char c)
        {
            if (c == '\t')
                return;

            io.AddInputCharacter(c);
        };
    }

    void ImGuiGraphicsRenderer::UpdateInput()
    {
        auto window = m_GraphicsDevice->GetPrimaryWindow();
        auto &io = ImGui::GetIO();

        auto keyboard = m_GraphicsDevice->GetPrimaryWindow()->GetInput()->GetKeyboard();
        auto mouse = m_GraphicsDevice->GetPrimaryWindow()->GetInput()->GetMouse();

        for (int i = 0; i < m_Keys.size(); i++)
        {
            io.KeysDown[m_Keys[i]] = keyboard.IsKeyHeld((KeyCode)m_Keys[i]);
        }

        io.KeyShift = keyboard.IsKeyHeld(KeyCode::LeftShift) || keyboard.IsKeyHeld(KeyCode::RightShift);
        io.KeyCtrl = keyboard.IsKeyHeld(KeyCode::LeftControl) || keyboard.IsKeyHeld(KeyCode::RightControl);
        io.KeyAlt = keyboard.IsKeyHeld(KeyCode::LeftAlt) || keyboard.IsKeyHeld(KeyCode::RightAlt);
        io.KeySuper = keyboard.IsKeyHeld(KeyCode::LeftWin) || keyboard.IsKeyHeld(KeyCode::RightWin);

        io.DisplaySize = {
            (float)window->GetWindowSize().X,
            (float)window->GetWindowSize().Y};
        io.DisplayFramebufferScale = {1, 1};
        io.MousePos = {(float)mouse.GetMousePosition().X, (float)mouse.GetMousePosition().Y};

        io.MouseDown[0] = mouse.IsLeftMouseHeld();
        io.MouseDown[1] = mouse.IsRightMouseHeld();
        io.MouseDown[2] = mouse.IsMiddleMouseHeld();

        io.MouseWheel = mouse.GetScrollMovement().Y;
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

        // update vertex buffer
        {
            uint32_t offset = 0;

            for (int i = 0; i < drawData->CmdListsCount; i++)
            {
                const ImDrawList *cmdList = drawData->CmdLists[i];
                m_VertexBuffer->SetData(cmdList->VtxBuffer.Data, cmdList->VtxBuffer.size_in_bytes(), offset);
                offset += cmdList->VtxBuffer.size_in_bytes();
            }
        }

        // update index buffer
        {
            uint32_t offset = 0;

            for (int i = 0; i < drawData->CmdListsCount; i++)
            {
                const ImDrawList *cmdList = drawData->CmdLists[i];
                m_IndexBuffer->SetData(cmdList->IdxBuffer.Data, cmdList->IdxBuffer.size_in_bytes(), offset);
                offset += cmdList->IdxBuffer.size_in_bytes();
            }
        }
    }

    void ImGuiGraphicsRenderer::RenderCommandLists(ImDrawData *drawData)
    {
        if (drawData->TotalVtxCount == 0)
            return;

        if (!m_VertexBuffer)
            return;

        if (!m_IndexBuffer)
            return;

        m_CommandList->Begin();
        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->SetVertexBuffer(m_VertexBuffer);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);

        auto &io = ImGui::GetIO();
        glm::mat4 mvp = glm::ortho<float>(0, io.DisplaySize.x, io.DisplaySize.y, 0, -1.f, 1.0f);
        m_UniformBuffer->SetData(&mvp, sizeof(mvp), 0);

        for (auto &resourceSet : m_ResourceSets)
        {
            resourceSet.second->WriteUniformBuffer(m_UniformBuffer, "MVP");
            resourceSet.second->PerformResourceUpdate();
        }

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

                    const auto &resourceSet = m_ResourceSets.at(drawCmd.TextureId);
                    m_CommandList->SetResourceSet(resourceSet);

                    m_CommandList->DrawIndexed(
                        drawCmd.ElemCount,
                        drawCmd.IdxOffset + idxOffset,
                        drawCmd.VtxOffset + vtxOffset);
                }
            }

            idxOffset += cmdList->IdxBuffer.Size;
            vtxOffset += cmdList->VtxBuffer.Size;
        }

        m_CommandList->End();

        m_GraphicsDevice->SubmitCommandList(m_CommandList);
    }

    void ImGuiGraphicsRenderer::UpdateCursor()
    {
        auto window = m_GraphicsDevice->GetPrimaryWindow();

        switch (ImGui::GetMouseCursor())
        {
        case ImGuiMouseCursor_Arrow:
        {
            window->SetCursor(Nexus::Cursor::Arrow);
            break;
        }
        case ImGuiMouseCursor_TextInput:
        {
            window->SetCursor(Nexus::Cursor::IBeam);
            break;
        case ImGuiMouseCursor_ResizeAll:
        {
            window->SetCursor(Nexus::Cursor::ArrowAllDir);
            break;
        }
        case ImGuiMouseCursor_ResizeNS:
        {
            window->SetCursor(Nexus::Cursor::ArrowNS);
            break;
        }
        case ImGuiMouseCursor_ResizeEW:
        {
            window->SetCursor(Nexus::Cursor::ArrowWE);
            break;
        }
        case ImGuiMouseCursor_ResizeNESW:
        {
            window->SetCursor(Nexus::Cursor::ArrowNESW);
            break;
        }
        case ImGuiMouseCursor_ResizeNWSE:
        {
            window->SetCursor(Nexus::Cursor::ArrowNWSE);
            break;
        }
        case ImGuiMouseCursor_Hand:
        {
            window->SetCursor(Nexus::Cursor::Hand);
            break;
        }
        case ImGuiMouseCursor_NotAllowed:
        {
            window->SetCursor(Nexus::Cursor::No);
            break;
        }
        }
        }
    }
}