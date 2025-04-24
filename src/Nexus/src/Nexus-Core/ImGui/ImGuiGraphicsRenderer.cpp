#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"

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
		ImGui::SetAllocatorFunctions(&ImGuiAlloc, &ImGuiFree, nullptr);

		Nexus::SetApplication(app);

		m_GraphicsDevice = app->GetGraphicsDevice();

		m_CommandList = m_GraphicsDevice->CreateCommandList();

		auto vertexSource	= GetImGuiShaderVertexSource();
		auto fragmentSource = GetImGuiShaderFragmentSource();

		m_VertexShader =
			m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvSource(vertexSource, "ImGui.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		m_FragmentShader =
			m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvSource(fragmentSource, "ImGui.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		CreateTextPipeline();
		CreateImagePipeline();

		Nexus::Graphics::DeviceBufferDescription uniformBufferDesc = {};
		uniformBufferDesc.Type									   = Nexus::Graphics::DeviceBufferType::Uniform;
		uniformBufferDesc.StrideInBytes							   = sizeof(glm::mat4);
		uniformBufferDesc.SizeInBytes							   = sizeof(glm::mat4);
		uniformBufferDesc.HostVisible							   = true;
		m_UniformBuffer = Ref<Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(uniformBufferDesc));

		Nexus::Graphics::SamplerSpecification samplerSpec;
		samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerSpec.MinimumLOD	 = 0;
		samplerSpec.MaximumLOD	 = 0;
		samplerSpec.LODBias		 = 0;
		samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
		m_Sampler				 = m_GraphicsDevice->CreateSampler(samplerSpec);

		m_Context = ImGui::CreateContext();
		SetCurrentRenderer(this);
		SetContext(m_Context);

		// look into implementing viewports
		UpdateMonitors();
	}

	ImGuiGraphicsRenderer::~ImGuiGraphicsRenderer()
	{
		if (s_ImGuiRenderer == this)
		{
			s_ImGuiRenderer = nullptr;
		}
	}

	void ImGuiGraphicsRenderer::CreateTextPipeline()
	{
		Nexus::Graphics::PipelineDescription pipelineDesc;

		pipelineDesc.VertexModule	= m_VertexShader;
		pipelineDesc.FragmentModule = m_FragmentShader;

		pipelineDesc.ColourFormats[0]		 = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
		pipelineDesc.ColourTargetCount		 = 1;
		pipelineDesc.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;
		pipelineDesc.DepthFormat			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;

		pipelineDesc.ColourBlendStates[0].EnableBlending		 = true;
		pipelineDesc.ColourBlendStates[0].SourceColourBlend		 = Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDesc.ColourBlendStates[0].DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDesc.ColourBlendStates[0].ColorBlendFunction	 = Nexus::Graphics::BlendEquation::Add;
		pipelineDesc.ColourBlendStates[0].SourceAlphaBlend		 = Nexus::Graphics::BlendFactor::One;
		pipelineDesc.ColourBlendStates[0].DestinationAlphaBlend	 = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDesc.ColourBlendStates[0].AlphaBlendFunction	 = Nexus::Graphics::BlendEquation::Add;

		pipelineDesc.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
		pipelineDesc.RasterizerStateDesc.TriangleFillMode  = Nexus::Graphics::FillMode::Solid;
		pipelineDesc.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDesc.DepthStencilDesc.DepthComparisonFunction	= Nexus::Graphics::ComparisonFunction::AlwaysPass;
		pipelineDesc.DepthStencilDesc.EnableDepthTest			= false;
		pipelineDesc.DepthStencilDesc.EnableDepthWrite			= false;
		pipelineDesc.DepthStencilDesc.EnableStencilTest			= false;
		pipelineDesc.DepthStencilDesc.StencilComparisonFunction = Nexus::Graphics::ComparisonFunction::AlwaysPass;

		pipelineDesc.Layouts = {{{Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}}};

		Nexus::Graphics::ResourceSetSpecification resources;
		resources += m_VertexShader->GetResourceSetSpecification();
		resources += m_FragmentShader->GetResourceSetSpecification();

		pipelineDesc.ResourceSetSpec = resources;
		m_TextPipeline				 = m_GraphicsDevice->CreatePipeline(pipelineDesc);
	}

	void ImGuiGraphicsRenderer::CreateImagePipeline()
	{
		Nexus::Graphics::PipelineDescription pipelineDesc;

		pipelineDesc.VertexModule	= m_VertexShader;
		pipelineDesc.FragmentModule = m_FragmentShader;

		pipelineDesc.ColourFormats[0]		 = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
		pipelineDesc.ColourTargetCount		 = 1;
		pipelineDesc.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;
		pipelineDesc.DepthFormat			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;

		pipelineDesc.ColourBlendStates[0].EnableBlending = false;

		pipelineDesc.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
		pipelineDesc.RasterizerStateDesc.TriangleFillMode  = Nexus::Graphics::FillMode::Solid;
		pipelineDesc.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDesc.DepthStencilDesc.DepthComparisonFunction	= Nexus::Graphics::ComparisonFunction::AlwaysPass;
		pipelineDesc.DepthStencilDesc.EnableDepthTest			= false;
		pipelineDesc.DepthStencilDesc.EnableDepthWrite			= false;
		pipelineDesc.DepthStencilDesc.EnableStencilTest			= false;
		pipelineDesc.DepthStencilDesc.StencilComparisonFunction = Nexus::Graphics::ComparisonFunction::AlwaysPass;

		pipelineDesc.Layouts = {{{Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
								 {Nexus::Graphics::ShaderDataType::NormByte4, "TEXCOORD"}}};

		Nexus::Graphics::ResourceSetSpecification resources;
		resources += m_VertexShader->GetResourceSetSpecification();
		resources += m_FragmentShader->GetResourceSetSpecification();

		pipelineDesc.ResourceSetSpec = resources;
		m_ImagePipeline				 = m_GraphicsDevice->CreatePipeline(pipelineDesc);
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

		auto resourceSet = m_GraphicsDevice->CreateResourceSet(m_TextPipeline);
		// resourceSet->WriteCombinedImageSampler(texture, m_Sampler, "Texture");

		m_Textures.insert({id, texture});
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

		auto windowSize			   = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();
		io.DisplaySize			   = {(float)windowSize.X, (float)windowSize.Y};
		io.DisplayFramebufferScale = {1, 1};

		UpdateMonitors();
		UpdateInput();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
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
					ImGuiWindowInfo			   *info	  = (ImGuiWindowInfo *)platform_io.Viewports[i]->PlatformUserData;
					Nexus::IWindow			   *window	  = info->Window;
					Nexus::Graphics::Swapchain *swapchain = info->Swapchain;

					if (window && !window->IsClosing())
					{
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

	ImGuiIO &ImGuiGraphicsRenderer::GetIO()
	{
		return ImGui::GetIO();
	}

	ImGuiContext *ImGuiGraphicsRenderer::GetContext()
	{
		return m_Context;
	}

	void ImGuiGraphicsRenderer::SetContext(ImGuiContext *context)
	{
		m_Context = context;
		ImGui::SetCurrentContext(context);
		ImGuizmo::SetImGuiContext(context);

		auto &io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

		if (m_GraphicsDevice->GetGraphicsCapabilities().SupportsMultipleSwapchains)
		{
			io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		}

		io.Fonts->AddFontDefault();
		RebuildFontAtlas();

		SetupHandlers();
		SetupInput(Nexus::GetApplication()->GetPrimaryWindow());
	}

	ImGuiGraphicsRenderer *ImGuiGraphicsRenderer::GetCurrentRenderer()
	{
		return s_ImGuiRenderer;
	}

	void ImGuiGraphicsRenderer::SetCurrentRenderer(ImGuiGraphicsRenderer *renderer)
	{
		s_ImGuiRenderer = renderer;
		ImGui::SetCurrentContext(s_ImGuiRenderer->GetContext());
		ImGui::SetAllocatorFunctions(&ImGuiAlloc, &ImGuiFree, nullptr);
	}

	void ImGuiGraphicsRenderer::SetupInput(IWindow *window)
	{
		auto &io = ImGui::GetIO();

		window->AddTextInputCallback(
			[&](const TextInputEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();
				io.AddInputCharactersUTF8(args.Text);
			});

		window->AddMouseScrollCallback(
			[&](const MouseScrolledEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();
				io.AddMouseWheelEvent(args.Scroll.X, args.Scroll.Y);
			});

		window->AddMousePressedCallback(
			[&](const Nexus::MouseButtonPressedEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();

				switch (args.Button)
				{
					case MouseButton::Left:
					{
						io.AddMouseButtonEvent(0, true);
						break;
					}
					case MouseButton::Right:
					{
						io.AddMouseButtonEvent(1, true);
						break;
					}
					case MouseButton::Middle:
					{
						io.AddMouseButtonEvent(2, true);
						break;
					}
				}

				Nexus::Point2D<float> position = args.Position;
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					position = args.ScreenPosition;
				}
				io.AddMousePosEvent(position.X, position.Y);
			});

		window->AddMouseReleasedCallback(
			[&](const Nexus::MouseButtonReleasedEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();

				switch (args.Button)
				{
					case MouseButton::Left:
					{
						io.AddMouseButtonEvent(0, false);
						break;
					}
					case MouseButton::Right:
					{
						io.AddMouseButtonEvent(1, false);
						break;
					}
					case MouseButton::Middle:
					{
						io.AddMouseButtonEvent(2, false);
						break;
					}
				}

				Nexus::Point2D<float> position = args.Position;
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					position = args.ScreenPosition;
				}
				io.AddMousePosEvent(position.X, position.Y);
			});

		window->AddMouseMovedCallback(
			[&](const Nexus::MouseMovedEventArgs &args)
			{
				ImGuiIO				 &io	   = ImGui::GetIO();
				Nexus::Point2D<float> position = args.Position;
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					position = args.ScreenPosition;
				}
				io.AddMousePosEvent(position.X, position.Y);
			});

		window->AddMouseScrollCallback(
			[&](const Nexus::MouseScrolledEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();
				io.AddMouseWheelEvent(args.Scroll.X, args.Scroll.Y);
			});
	}

	void ImGuiGraphicsRenderer::UpdateInput()
	{
		auto  mainWindow = Nexus::GetApplication()->GetPrimaryWindow();
		auto &io		 = ImGui::GetIO();

		std::optional<IWindow *> window = Platform::GetActiveWindow();
		if (!window.has_value())
		{
			return;
		}

		std::vector<IWindow *> &windows = Platform::GetWindows();

		for (auto w : windows)
		{
			if (io.WantTextInput)
			{
				w->StartTextInput();
			}
			else
			{
				w->StopTextInput();
			}
		}

		IWindow *activeWindow = window.value();
		io.AddKeyEvent(ImGuiKey_Tab, activeWindow->IsKeyDown(Nexus::ScanCode::Tab));
		io.AddKeyEvent(ImGuiKey_LeftArrow, activeWindow->IsKeyDown(Nexus::ScanCode::Left));
		io.AddKeyEvent(ImGuiKey_RightArrow, activeWindow->IsKeyDown(Nexus::ScanCode::Right));
		io.AddKeyEvent(ImGuiKey_UpArrow, activeWindow->IsKeyDown(Nexus::ScanCode::Up));
		io.AddKeyEvent(ImGuiKey_DownArrow, activeWindow->IsKeyDown(Nexus::ScanCode::Down));
		io.AddKeyEvent(ImGuiKey_PageUp, activeWindow->IsKeyDown(Nexus::ScanCode::PageUp));
		io.AddKeyEvent(ImGuiKey_PageDown, activeWindow->IsKeyDown(Nexus::ScanCode::PageDown));
		io.AddKeyEvent(ImGuiKey_Home, activeWindow->IsKeyDown(Nexus::ScanCode::Home));
		io.AddKeyEvent(ImGuiKey_End, activeWindow->IsKeyDown(Nexus::ScanCode::End));
		io.AddKeyEvent(ImGuiKey_Delete, activeWindow->IsKeyDown(Nexus::ScanCode::Delete));
		io.AddKeyEvent(ImGuiKey_Backspace, activeWindow->IsKeyDown(Nexus::ScanCode::Backspace));
		io.AddKeyEvent(ImGuiKey_Enter, activeWindow->IsKeyDown(Nexus::ScanCode::Return));
		io.AddKeyEvent(ImGuiKey_Escape, activeWindow->IsKeyDown(Nexus::ScanCode::Escape));
		io.AddKeyEvent(ImGuiKey_Space, activeWindow->IsKeyDown(Nexus::ScanCode::Space));
		io.AddKeyEvent(ImGuiKey_A, activeWindow->IsKeyDown(Nexus::ScanCode::A));
		io.AddKeyEvent(ImGuiKey_C, activeWindow->IsKeyDown(Nexus::ScanCode::C));
		io.AddKeyEvent(ImGuiKey_V, activeWindow->IsKeyDown(Nexus::ScanCode::V));
		io.AddKeyEvent(ImGuiKey_X, activeWindow->IsKeyDown(Nexus::ScanCode::X));
		io.AddKeyEvent(ImGuiKey_Y, activeWindow->IsKeyDown(Nexus::ScanCode::Y));
		io.AddKeyEvent(ImGuiKey_Z, activeWindow->IsKeyDown(Nexus::ScanCode::Z));

		io.KeyShift = activeWindow->IsKeyDown(ScanCode::LeftShift) || activeWindow->IsKeyDown(ScanCode::RightShift);
		io.KeyCtrl	= activeWindow->IsKeyDown(ScanCode::LeftControl) || activeWindow->IsKeyDown(ScanCode::RightControl);
		io.KeyAlt	= activeWindow->IsKeyDown(ScanCode::LeftAlt) || activeWindow->IsKeyDown(ScanCode::RightAlt);
		io.KeySuper = activeWindow->IsKeyDown(ScanCode::LeftGUI) || activeWindow->IsKeyDown(ScanCode::RightGUI);

		MouseState			  state	   = Platform::GetMouseState();
		Nexus::Point2D<float> mousePos = activeWindow->GetMousePosition();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			mousePos = state.MousePosition;
		}

		io.AddMousePosEvent(mousePos.X, mousePos.Y);
		io.AddMouseButtonEvent(0, state.LeftButton == MouseButtonState::Pressed);
		io.AddMouseButtonEvent(1, state.RightButton == MouseButtonState::Pressed);
		io.AddMouseButtonEvent(2, state.MiddleButton == MouseButtonState::Pressed);
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
			m_VertexBufferCount = drawData->TotalVtxCount * 1.5f;

			Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
			vertexBufferDesc.Type									  = Nexus::Graphics::DeviceBufferType::Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(ImDrawVert);
			vertexBufferDesc.SizeInBytes							  = m_VertexBufferCount * sizeof(ImDrawVert);
			vertexBufferDesc.HostVisible							  = true;
			m_VertexBuffer = Ref<Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
		}

		if (drawData->TotalIdxCount > m_IndexBufferCount)
		{
			m_IndexBufferCount = drawData->TotalIdxCount * 1.5f;

			Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
			indexBufferDesc.Type									 = Nexus::Graphics::DeviceBufferType::Index;
			indexBufferDesc.StrideInBytes							 = sizeof(ImDrawIdx);
			indexBufferDesc.SizeInBytes								 = m_IndexBufferCount * sizeof(ImDrawVert);
			indexBufferDesc.HostVisible								 = true;
			m_IndexBuffer = Ref<Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc));
		}

		// update vertex buffer
		{
			uint32_t offset = 0;

			for (int i = 0; i < drawData->CmdListsCount; i++)
			{
				const ImDrawList *cmdList = drawData->CmdLists[i];
				m_VertexBuffer->SetData(cmdList->VtxBuffer.Data, offset, cmdList->VtxBuffer.size_in_bytes());
				offset += cmdList->VtxBuffer.size_in_bytes();
			}
		}

		// update index buffer
		{
			uint32_t offset = 0;

			for (int i = 0; i < drawData->CmdListsCount; i++)
			{
				const ImDrawList *cmdList = drawData->CmdLists[i];
				m_IndexBuffer->SetData(cmdList->IdxBuffer.Data, offset, cmdList->IdxBuffer.size_in_bytes());
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

		ImVec2 pos = drawData->DisplayPos;

		auto	 &io  = ImGui::GetIO();
		glm::mat4 mvp = glm::ortho<float>(pos.x, pos.x + drawData->DisplaySize.x, pos.y + drawData->DisplaySize.y, pos.y, -1.f, 1.0f);
		m_UniformBuffer->SetData(&mvp, 0, sizeof(mvp));

		for (auto &[textureId, resourceSet] : m_ResourceSets)
		{
			Ref<Graphics::Texture2D> texture = m_Textures.at(textureId);

			Graphics::UniformBufferView uniformBufferView = {};
			uniformBufferView.BufferHandle				  = m_UniformBuffer.get();
			uniformBufferView.Offset					  = 0;
			uniformBufferView.Size						  = m_UniformBuffer->GetDescription().SizeInBytes;
			resourceSet->WriteUniformBuffer(uniformBufferView, "MVP");
			resourceSet->WriteCombinedImageSampler(texture, m_Sampler, "Texture");
		}

		ImGuiViewport *vp = drawData->OwnerViewport;

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
					if (drawCmd.TextureId == m_FontTextureID)
					{
						m_CommandList->SetPipeline(m_TextPipeline);
					}
					else
					{
						m_CommandList->SetPipeline(m_ImagePipeline);
					}
					m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(info->Swapchain));

					Graphics::VertexBufferView vertexBufferView = {};
					vertexBufferView.BufferHandle				= m_VertexBuffer.get();
					vertexBufferView.Offset						= 0;
					m_CommandList->SetVertexBuffer(vertexBufferView, 0);

					Graphics::IndexBufferView indexBufferView = {};
					indexBufferView.BufferHandle			  = m_IndexBuffer.get();
					indexBufferView.Offset					  = 0;
					indexBufferView.BufferFormat			  = Graphics::IndexBufferFormat::UInt16;
					m_CommandList->SetIndexBuffer(indexBufferView);

					Nexus::Graphics::Viewport viewport;
					viewport.X		= 0;
					viewport.Y		= 0;
					viewport.Width	= drawData->DisplaySize.x;
					viewport.Height = drawData->DisplaySize.y;
					m_CommandList->SetViewport(viewport);

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
		auto window = Nexus::GetApplication()->GetPrimaryWindow();

		switch (ImGui::GetMouseCursor())
		{
			case ImGuiMouseCursor_Arrow:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::Arrow);
				break;
			}
			case ImGuiMouseCursor_TextInput:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::IBeam);
				break;
			}
			case ImGuiMouseCursor_ResizeAll:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::ArrowAllDir);
				break;
			}
			case ImGuiMouseCursor_ResizeNS:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::ArrowNS);
				break;
			}
			case ImGuiMouseCursor_ResizeEW:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::ArrowWE);
				break;
			}
			case ImGuiMouseCursor_ResizeNESW:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::ArrowNESW);
				break;
			}
			case ImGuiMouseCursor_ResizeNWSE:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::ArrowNWSE);
				break;
			}
			case ImGuiMouseCursor_Hand:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::Hand);
				break;
			}
			case ImGuiMouseCursor_NotAllowed:
			{
				Platform::SetCursor(Nexus::Platform::Cursor::No);
				break;
			}
		}
	}

	void ImGuiGraphicsRenderer::UpdateMonitors()
	{
		auto &platformIo = ImGui::GetPlatformIO();
		platformIo.Monitors.clear();

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

	void ImGuiGraphicsRenderer::SetupHandlers()
	{
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

			Nexus::Graphics::SwapchainSpecification swapchainSpec = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification();

			Nexus::IWindow			   *window	  = Platform::CreatePlatformWindow(windowSpec);
			Nexus::Graphics::Swapchain *swapchain = app->GetGraphicsDevice()->CreateSwapchain(window, swapchainSpec);
			swapchain->Initialise();
			window->SetWindowPosition(vp->Pos.x, vp->Pos.y);

			ImGuiWindowInfo *info = new ImGuiWindowInfo();
			info->Window		  = window;
			info->Swapchain		  = swapchain;

			Nexus::ImGuiUtils::ImGuiGraphicsRenderer::SetupInput(window);

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
			return 1.0f;
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
		info->Window		  = m_Application->GetPrimaryWindow();
		info->Swapchain		  = m_Application->GetPrimarySwapchain();
		vp->PlatformUserData  = info;
		vp->RendererUserData  = info;
	}

}	 // namespace Nexus::ImGuiUtils
