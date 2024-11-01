#include "ImGuiGraphicsRenderer.hpp"

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/Runtime.hpp"

std::string GetImGuiShaderVertexSource()
{
	std::string shader = "#version 450 core\n"

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
	std::string shader = "#version 450 core\n"

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

static Nexus::ImGuiUtils::ImGuiGraphicsRenderer *s_ImGuiRenderer = nullptr;

static void ImGui_ImplNexus_SetPlatformImeData(ImGuiViewport *vp, ImGuiPlatformImeData *data)
{
	Nexus::ImGuiUtils::ImGuiWindowInfo *info = (Nexus::ImGuiUtils::ImGuiWindowInfo *)vp->PlatformUserData;
	if (data->WantVisible)
	{
		Nexus::Graphics::Rectangle<int> rect = {(int)data->InputPos.x, (int)data->InputPos.y, 1, (int)data->InputLineHeight};

		info->Window->SetTextInputRect(rect);
		info->Window->StartTextInput();
	}
	else
	{
		info->Window->StopTextInput();
	}
}

namespace Nexus::ImGuiUtils
{
	ImGuiGraphicsRenderer::ImGuiGraphicsRenderer(Nexus::Application *app) : m_Application(app)
	{
		s_ImGuiRenderer = this;

		m_GraphicsDevice = app->GetGraphicsDevice();

		m_CommandList = m_GraphicsDevice->CreateCommandList();

		auto vertexSource	= GetImGuiShaderVertexSource();
		auto fragmentSource = GetImGuiShaderFragmentSource();

		auto vertexModule =
		m_GraphicsDevice->CreateShaderModuleFromSpirvSource(vertexSource, "ImGui.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		auto fragmentModule =
		m_GraphicsDevice->CreateShaderModuleFromSpirvSource(fragmentSource, "ImGui.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		Nexus::Graphics::PipelineDescription pipelineDesc;

		pipelineDesc.VertexModule	= vertexModule;
		pipelineDesc.FragmentModule = fragmentModule;

		pipelineDesc.ColourFormats[0]		 = Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDesc.ColourTargetCount		 = 1;
		pipelineDesc.ColourTargetSampleCount = m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->GetSpecification().Samples;
		pipelineDesc.DepthFormat			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;

		pipelineDesc.BlendStateDesc.EnableBlending		   = true;
		pipelineDesc.BlendStateDesc.SourceColourBlend	   = Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDesc.BlendStateDesc.DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDesc.BlendStateDesc.ColorBlendFunction	   = Nexus::Graphics::BlendEquation::Add;
		pipelineDesc.BlendStateDesc.SourceAlphaBlend	   = Nexus::Graphics::BlendFactor::One;
		pipelineDesc.BlendStateDesc.DestinationAlphaBlend  = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDesc.BlendStateDesc.AlphaBlendFunction	   = Nexus::Graphics::BlendEquation::Add;

		pipelineDesc.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::None;
		pipelineDesc.RasterizerStateDesc.TriangleFillMode  = Nexus::Graphics::FillMode::Solid;
		pipelineDesc.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDesc.DepthStencilDesc.DepthComparisonFunction	= Nexus::Graphics::ComparisonFunction::Always;
		pipelineDesc.DepthStencilDesc.EnableDepthTest			= false;
		pipelineDesc.DepthStencilDesc.EnableDepthWrite			= false;
		pipelineDesc.DepthStencilDesc.EnableStencilTest			= false;
		pipelineDesc.DepthStencilDesc.StencilComparisonFunction = Nexus::Graphics::ComparisonFunction::Always;

		pipelineDesc.Layouts = {{{Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}}};

		Nexus::Graphics::BufferDescription uniformBufferDesc;
		uniformBufferDesc.Size	= sizeof(glm::mat4);
		uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
		m_UniformBuffer			= m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

		Nexus::Graphics::ResourceSetSpecification resources;
		resources += vertexModule->GetResourceSetSpecification();
		resources += fragmentModule->GetResourceSetSpecification();

		pipelineDesc.ResourceSetSpec = resources;

		m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDesc);

		Nexus::Graphics::SamplerSpecification samplerSpec;
		samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.MinimumLOD	 = 0;
		samplerSpec.MaximumLOD	 = 0;
		samplerSpec.LODBias		 = 0;
		samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
		m_Sampler				 = m_GraphicsDevice->CreateSampler(samplerSpec);

		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		auto &io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

		io.SetPlatformImeDataFn = ImGui_ImplNexus_SetPlatformImeData;

		if (m_GraphicsDevice->GetGraphicsCapabilities().SupportsMultipleSwapchains)
		{
			io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		}

		io.Fonts->AddFontDefault();

		SetupInput();

		// look into implementing viewports
		UpdateMonitors();

		ImGuiPlatformIO &platformIo = ImGui::GetPlatformIO();

		// platform
		platformIo.Platform_CreateWindow = [](ImGuiViewport *vp) -> void
		{
			auto app			= Nexus::GetApplication();
			auto graphicsDevice = app->GetGraphicsDevice();

			WindowSpecification windowSpec;
			windowSpec.Width	  = vp->Size.x;
			windowSpec.Height	  = vp->Size.y;
			windowSpec.Borderless = true;

			Nexus::Graphics::SwapchainSpecification swapchainSpec = app->GetPrimaryWindow()->GetSwapchain()->GetSpecification();

			Nexus::Window *window = Platform::CreatePlatformWindow(windowSpec, app->GetGraphicsDevice()->GetGraphicsAPI(), swapchainSpec);
			window->CreateSwapchain(app->GetGraphicsDevice(), swapchainSpec);
			window->GetSwapchain()->Initialise();
			window->SetWindowPosition(vp->Pos.x, vp->Pos.y);

			ImGuiWindowInfo *info = new ImGuiWindowInfo();
			info->Window		  = window;

			vp->PlatformUserData = info;
			vp->RendererUserData = info;
		};

		platformIo.Platform_DestroyWindow = [](ImGuiViewport *vp) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;
				delete info;

				vp->PlatformUserData = nullptr;
				vp->RendererUserData = nullptr;
			}
		};

		platformIo.Platform_ShowWindow = [](ImGuiViewport *vp) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					info->Window->Show();
				}
			}
		};

		platformIo.Platform_SetWindowPos = [](ImGuiViewport *vp, ImVec2 pos) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					info->Window->SetWindowPosition(pos.x, pos.y);
				}
			}
		};

		platformIo.Platform_SetWindowSize = [](ImGuiViewport *vp, ImVec2 size) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					info->Window->SetSize({(uint32_t)size.x, (uint32_t)size.y});
				}
			}
		};

		platformIo.Platform_SetWindowFocus = [](ImGuiViewport *vp) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					info->Window->Focus();
				}
			}
		};

		platformIo.Platform_GetWindowFocus = [](ImGuiViewport *vp) -> bool
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					return info->Window->IsFocussed();
				}
			}
			return false;
		};

		platformIo.Platform_GetWindowMinimized = [](ImGuiViewport *vp) -> bool
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;
				if (!info->Window->IsClosing())
				{
					return info->Window->GetCurrentWindowState() == Nexus::WindowState::Minimized;
				}
			}
			return false;
		};

		platformIo.Platform_SetWindowTitle = [](ImGuiViewport *vp, const char *str) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;
				if (!info->Window->IsClosing())
				{
					info->Window->SetTitle({str});
				}
			}
		};

		platformIo.Platform_GetWindowDpiScale = [](ImGuiViewport *vp) -> float
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;
				if (!info->Window->IsClosing())
				{
					return info->Window->GetDisplayScale();
				}
			}
			return 0.0f;
		};

		platformIo.Platform_GetWindowPos = [](ImGuiViewport *vp) -> ImVec2
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					auto pos = info->Window->GetWindowPosition();
					return {(float)pos.X, (float)pos.Y};
				}
			}
			return {0, 0};
		};

		platformIo.Platform_GetWindowSize = [](ImGuiViewport *vp) -> ImVec2
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (!info->Window->IsClosing())
				{
					auto size = info->Window->GetWindowSize();
					return {(float)size.X, (float)size.Y};
				}
			}
			return {0, 0};
		};

		ImGuiViewport *vp = ImGui::GetMainViewport();

		ImGuiWindowInfo *info = new ImGuiWindowInfo();
		info->Window		  = app->GetPrimaryWindow();
		vp->PlatformUserData  = info;
		vp->RendererUserData  = info;
	}

	ImGuiGraphicsRenderer::~ImGuiGraphicsRenderer()
	{
		if (s_ImGuiRenderer == this)
		{
			s_ImGuiRenderer = nullptr;
		}
	}

	void ImGuiGraphicsRenderer::RebuildFontAtlas()
	{
		auto		  &io = ImGui::GetIO();
		unsigned char *pixels;
		int			   width, height, channels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &channels);

		Nexus::Graphics::Texture2DSpecification spec;
		spec.Width	= width;
		spec.Height = height;
		spec.Format = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;

		m_FontTexture = m_GraphicsDevice->CreateTexture2D(spec);
		m_FontTexture->SetData(pixels, 0, 0, 0, width, height);

		UnbindTexture(m_FontTextureID);

		m_FontTextureID = BindTexture(m_FontTexture);
		io.Fonts->SetTexID(m_FontTextureID);
		io.Fonts->ClearTexData();
	}

	ImTextureID ImGuiGraphicsRenderer::BindTexture(Nexus::Ref<Nexus::Graphics::Texture2D> texture)
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

	void ImGuiGraphicsRenderer::BeforeLayout(Nexus::TimeSpan gameTime)
	{
		auto &io	 = ImGui::GetIO();
		io.DeltaTime = (float)gameTime.GetSeconds();

		auto windowSize			   = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize();
		io.DisplaySize			   = {(float)windowSize.X, (float)windowSize.Y};
		io.DisplayFramebufferScale = {1, 1};

		UpdateInput();
		ImGui::NewFrame();
	}

	void ImGuiGraphicsRenderer::AfterLayout()
	{
		ImGui::Render();

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();

			const ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();

			// iterating through the viewports backwards seems to solve a rendering
			// issue
			for (int i = platform_io.Viewports.Size - 1; i >= 1; i--)
			{
				if ((platform_io.Viewports[i]->Flags & ImGuiViewportFlags_IsMinimized) == 0)
				{
					ImGuiWindowInfo *info	= (ImGuiWindowInfo *)platform_io.Viewports[i]->PlatformUserData;
					Nexus::Window	*window = info->Window;

					if (window && !window->IsClosing())
					{
						Nexus::Graphics::Swapchain *swapchain = window->GetSwapchain();
						if (swapchain)
						{
							swapchain->Prepare();
							RenderDrawData(platform_io.Viewports[i]->DrawData);
							swapchain->SwapBuffers();
						}
					}
				}
			}
		}

		RenderDrawData(ImGui::GetDrawData());
		UpdateCursor();
	}

	void ImGuiGraphicsRenderer::SetupInput()
	{
		auto &io = ImGui::GetIO();

		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Tab] = (int)ScanCode::Tab);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_LeftArrow] = (int)ScanCode::Left);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_RightArrow] = (int)ScanCode::Right);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_UpArrow] = (int)ScanCode::Up);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_DownArrow] = (int)ScanCode::Down);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_PageUp] = (int)ScanCode::PageUp);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_PageDown] = (int)ScanCode::PageDown);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Home] = (int)ScanCode::Home);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_End] = (int)ScanCode::End);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Delete] = (int)ScanCode::Delete);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Backspace] = (int)ScanCode::Backspace);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Enter] = (int)ScanCode::Return);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Escape] = (int)ScanCode::Escape);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Space] = (int)ScanCode::Space);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_A] = (int)ScanCode::A);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_C] = (int)ScanCode::C);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_V] = (int)ScanCode::V);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_X] = (int)ScanCode::X);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Y] = (int)ScanCode::Y);
		m_Keys.push_back(io.KeyMap[(int)ImGuiKey_Z] = (int)ScanCode::Z);

		m_Application->GetPrimaryWindow()->OnTextInput.Bind(
		[&](const TextInputEventArgs &args)
		{
			ImGuiIO &io = ImGui::GetIO();
			io.AddInputCharactersUTF8(args.Text);
		});
	}

	void ImGuiGraphicsRenderer::UpdateInput()
	{
		auto  mainWindow = m_GraphicsDevice->GetPrimaryWindow();
		auto &io		 = ImGui::GetIO();

		std::optional<Window *> window = Platform::GetKeyboardFocus();
		if (!window.has_value())
		{
			return;
		}

		InputNew::InputContext *context = window.value()->GetInputContext();

		for (int i = 0; i < m_Keys.size(); i++) { io.KeysDown[m_Keys[i]] = context->IsKeyDown(0, (ScanCode)m_Keys[i]); }

		io.KeyShift = context->IsKeyDown(0, ScanCode::LeftShift) || context->IsKeyDown(0, ScanCode::RightShift);
		io.KeyCtrl	= context->IsKeyDown(0, ScanCode::LeftControl) || context->IsKeyDown(0, ScanCode::RightControl);
		io.KeyAlt	= context->IsKeyDown(0, ScanCode::LeftAlt) || context->IsKeyDown(0, ScanCode::RightAlt);
		io.KeySuper = context->IsKeyDown(0, ScanCode::LeftGUI) || context->IsKeyDown(0, ScanCode::RightGUI);

		io.DisplaySize			   = {(float)mainWindow->GetWindowSize().X, (float)mainWindow->GetWindowSize().Y};
		io.DisplayFramebufferScale = {1, 1};

		InputNew::MouseInfo globalMouseState = Platform::GetGlobalMouseInfo();

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			io.MousePos = {(float)globalMouseState.Position.X, (float)globalMouseState.Position.Y};
		}
		else
		{
			io.MousePos = {(float)context->GetCursorPosition().X, (float)context->GetCursorPosition().Y};
		}

		io.MouseDown[0] = globalMouseState.Buttons[MouseButton::Left] == MouseButtonState::Pressed;
		io.MouseDown[1] = globalMouseState.Buttons[MouseButton::Right] == MouseButtonState::Pressed;
		io.MouseDown[2] = globalMouseState.Buttons[MouseButton::Middle] == MouseButtonState::Pressed;

		io.MouseWheel = context->GetScroll(0).Y;
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
			Nexus::Graphics::VertexBufferLayout layout = {{Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														  {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														  {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}};

			Nexus::Graphics::BufferDescription desc;
			desc.Size	   = drawData->TotalVtxCount * 1.5f * sizeof(ImDrawVert);
			desc.Usage	   = Nexus::Graphics::BufferUsage::Dynamic;
			m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(desc, nullptr);

			m_VertexBufferCount = drawData->TotalVtxCount * 1.5f;
		}

		if (drawData->TotalIdxCount > m_IndexBufferCount)
		{
			Nexus::Graphics::BufferDescription desc;
			desc.Size	  = drawData->TotalIdxCount * 1.5f * sizeof(ImDrawIdx);
			desc.Usage	  = Nexus::Graphics::BufferUsage::Dynamic;
			m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(desc, nullptr, Nexus::Graphics::IndexBufferFormat::UInt16);

			m_IndexBufferCount = drawData->TotalIdxCount * 1.5f;
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

		ImGuiWindowInfo *info = (ImGuiWindowInfo *)drawData->OwnerViewport->PlatformUserData;

		m_CommandList->Begin();
		m_CommandList->SetPipeline(m_Pipeline);
		m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(info->Window->GetSwapchain()));
		m_CommandList->SetVertexBuffer(m_VertexBuffer, 0);
		m_CommandList->SetIndexBuffer(m_IndexBuffer);

		ImVec2 pos = drawData->DisplayPos;

		auto	 &io  = ImGui::GetIO();
		glm::mat4 mvp = glm::ortho<float>(pos.x, pos.x + drawData->DisplaySize.x, pos.y + drawData->DisplaySize.y, pos.y, -1.f, 1.0f);
		m_UniformBuffer->SetData(&mvp, sizeof(mvp), 0);

		for (auto &resourceSet : m_ResourceSets) { resourceSet.second->WriteUniformBuffer(m_UniformBuffer, "MVP"); }

		ImGuiViewport *vp = drawData->OwnerViewport;

		Nexus::Graphics::Viewport viewport;
		viewport.X		= 0;
		viewport.Y		= 0;
		viewport.Width	= drawData->DisplaySize.x;
		viewport.Height = drawData->DisplaySize.y;
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
					scissor.X	   = drawCmd.ClipRect.x - pos.x;
					scissor.Y	   = drawCmd.ClipRect.y - pos.y;
					scissor.Width  = (uint32_t)(drawCmd.ClipRect.z - drawCmd.ClipRect.x);
					scissor.Height = (uint32_t)(drawCmd.ClipRect.w - drawCmd.ClipRect.y);
					m_CommandList->SetScissor(scissor);

					const auto &resourceSet = m_ResourceSets.at(drawCmd.TextureId);
					m_CommandList->SetResourceSet(resourceSet);

					m_CommandList->DrawIndexed(drawCmd.ElemCount, drawCmd.IdxOffset + idxOffset, drawCmd.VtxOffset + vtxOffset);
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

	void ImGuiGraphicsRenderer::UpdateMonitors()
	{
		auto &platformIo = ImGui::GetPlatformIO();
		platformIo.Monitors.resize(0);

		std::vector<Monitor> monitors = Nexus::Platform::GetMonitors();

		for (const auto &monitor : monitors)
		{
			ImGuiPlatformMonitor m;
			m.DpiScale = monitor.DPI;
			m.MainPos  = {(float)monitor.Position.X, (float)monitor.Position.Y};
			m.MainSize = {(float)monitor.Size.X, (float)monitor.Size.Y};
			m.WorkPos  = {(float)monitor.WorkPosition.X, (float)monitor.WorkPosition.Y};
			m.WorkSize = {(float)monitor.WorkSize.X, (float)monitor.WorkSize.Y};

			platformIo.Monitors.push_back(m);
		}
	}
}	 // namespace Nexus::ImGuiUtils