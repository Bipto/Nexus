#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Platform.hpp"

#include "Nexus-Core/Graphics/PixelFormat.hpp"
#include "Nexus-Core/Runtime.hpp"

#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Platform/Timings/Profiler.hpp"

static std::string GetImGuiShaderVertexSource()
{
	std::string shader = "#version 450 core\n"

						 "layout(location = 0) in vec2 a_Position;\n"
						 "layout(location = 1) in vec2 a_TexCoord;\n"
						 "layout(location = 2) in vec4 a_Colour;\n"

						 "layout(location = 0) out vec2 o_Frag_UV;\n"
						 "layout(location = 1) out vec4 o_Frag_Colour;\n"

						 "layout(push_constant) uniform PushConstants\n"
						 "{\n"
						 "    mat4 MVP;\n"
						 "} pushConstants;\n"

						 "void main()\n"
						 "{\n"
						 "    gl_Position = pushConstants.MVP * vec4(a_Position, 0.0, 1.0);\n"
						 "    o_Frag_UV = vec2(a_TexCoord.x, a_TexCoord.y);\n"
						 "    o_Frag_Colour = a_Colour;\n"
						 "}";
	return shader;
}

static std::string GetImGuiShaderFragmentSource()
{
	std::string shader = "#version 450 core\n"

						 "layout(location = 0) in vec2 a_Frag_UV;\n"
						 "layout(location = 1) in vec4 a_Frag_Colour;\n"

						 "layout(set = 1, binding = 0) uniform sampler2D u_Texture;\n"

						 "layout(location = 0) out vec4 o_Colour;\n"

						 "void main()\n"
						 "{\n"
						 "    o_Colour = a_Frag_Colour * texture(u_Texture, a_Frag_UV.st)\n;"
						 "}";
	return shader;
}

static Nexus::ImGuiUtils::ImGuiGraphicsRenderer *s_ImGuiRenderer = nullptr;

static void ImGui_ImplNexus_SetPlatformImeData(ImGuiViewport *vp, ImGuiPlatformImeData *data)
{
	Nexus::ImGuiUtils::ImGuiWindowInfo *info = static_cast<Nexus::ImGuiUtils::ImGuiWindowInfo *>(vp->PlatformUserData);
	if (data->WantVisible)
	{
		info->Window->SetTextInputRect(data->InputPos.x, data->InputPos.y, 1, data->InputLineHeight);
		info->Window->StartTextInput();
	}
	else
	{
		info->Window->StopTextInput();
	}
}

namespace Nexus::ImGuiUtils
{
	ImGuiGraphicsRenderer::ImGuiGraphicsRenderer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
		: m_Application(app),
		  m_CommandQueue(commandQueue)
	{
		s_ImGuiRenderer = this;
		ImGui::SetAllocatorFunctions(&ImGuiAlloc, &ImGuiFree, nullptr);

		Nexus::SetApplication(app);

		m_GraphicsDevice = app->GetGraphicsDevice();

		Nexus::Graphics::CommandListDescription commandListDesc = {};
		commandListDesc.DebugName								= "ImGui CommandList";
		m_CommandList											= m_CommandQueue->CreateCommandList(commandListDesc);

		auto vertexSource	= GetImGuiShaderVertexSource();
		auto fragmentSource = GetImGuiShaderFragmentSource();

		m_VertexShader	 = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_GraphicsDevice,
																				vertexSource,
																				"ImGui.vert.glsl",
																				Nexus::GetApplication()->GetApplicationPath(),
																				Nexus::Graphics::ShaderStage::Vertex);
		m_FragmentShader = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_GraphicsDevice,
																				fragmentSource,
																				"ImGui.frag.glsl",
																				Nexus::GetApplication()->GetApplicationPath(),
																				Nexus::Graphics::ShaderStage::Fragment);

		CreatePipeline();

		Nexus::Graphics::SamplerDescription samplerDesc;
		samplerDesc.AddressModeU = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerDesc.AddressModeV = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerDesc.AddressModeW = Nexus::Graphics::SamplerAddressMode::Wrap;
		samplerDesc.MinimumLOD	 = 0;
		samplerDesc.MaximumLOD	 = 0;
		samplerDesc.LODBias		 = 0;
		samplerDesc.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
		samplerDesc.DebugName	 = "ImGui Sampler";
		m_Sampler				 = m_GraphicsDevice->CreateSampler(samplerDesc);

		m_Context = ImGui::CreateContext();
		SetCurrentRenderer(this);
		SetContext(m_Context);
		UpdateMonitors();
	}

	ImGuiGraphicsRenderer::~ImGuiGraphicsRenderer()
	{
		ImGui::Shutdown();
	}

	void ImGuiGraphicsRenderer::CreatePipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDesc;

		pipelineDesc.VertexModule	= m_VertexShader;
		pipelineDesc.FragmentModule = m_FragmentShader;

		pipelineDesc.ColourFormats[0]  = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
		pipelineDesc.ColourTargetCount = 1;
		pipelineDesc.Samples		   = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

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

		pipelineDesc.DepthFormat = Nexus::GetApplication()->GetPrimarySwapchain()->GetDepthFormat();

		pipelineDesc.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::AlwaysPass;
		pipelineDesc.DepthStencilDesc.EnableDepthTest		  = false;
		pipelineDesc.DepthStencilDesc.EnableDepthWrite		  = false;
		pipelineDesc.DepthStencilDesc.EnableStencilTest		  = false;

		pipelineDesc.Layouts = {
			Nexus::Graphics::VertexBufferLayout({Nexus::Graphics::VertexBufferElement(Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"),
												 Nexus::Graphics::VertexBufferElement(Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"),
												 Nexus::Graphics::VertexBufferElement(Nexus::Graphics::ShaderDataType::R8G8B8A8_UNorm, "TEXCOORD")},
												sizeof(ImDrawVert),
												Nexus::Graphics::StepRate::Vertex)};

		pipelineDesc.DebugName = "ImGui Pipeline";

		pipelineDesc.ResourceDescription.Descriptors = {Graphics::ResourceDescriptor {.Name = "PushConstants",
																					  .Type = Graphics::ResourceDescriptorType::PushConstants,
																					  .CountOrSizeInBytes = sizeof(glm::mat4)},
														{Graphics::ResourceDescriptor {.Name = "u_Texture",
																					   .Type = Graphics::ResourceDescriptorType::CombinedImageSampler,
																					   .CountOrSizeInBytes = 1}}};

		m_Pipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDesc);
	}

	void ImGuiGraphicsRenderer::SetupInput(IWindow *window)
	{
		auto &io = ImGui::GetIO();

		/*window->AddTextInputCallback(
			[&](const TextInputEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();
				io.AddInputCharactersUTF8(args.Text);
			});

		window->AddMouseScrollCallback(
			[&](const MouseScrolledEventArgs &args)
			{
				ImGuiIO &io = ImGui::GetIO();
				auto [x, y] = args.Scroll;
				io.AddMouseWheelEvent(x, y);
			});*/
	}

	void ImGuiGraphicsRenderer::RebuildFontAtlas()
	{
		auto		  &io = ImGui::GetIO();
		unsigned char *pixels;
		int			   width, height, channels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &channels);

		size_t bufferSize =
			static_cast<size_t>(width) * static_cast<size_t>(height) * Graphics::GetPixelFormatSizeInBytes(Graphics::PixelFormat::R8_G8_B8_A8_UNorm);

		Graphics::TextureDescription textureDesc = {};
		textureDesc.Type						 = Graphics::TextureType::Texture2D;
		textureDesc.Width						 = width;
		textureDesc.Height						 = height;
		textureDesc.Format						 = Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		textureDesc.Usage						 = Graphics::TextureUsage_Sampled;
		textureDesc.DebugName					 = "ImGui Font Texture";
		m_FontTexture							 = m_GraphicsDevice->CreateTexture(textureDesc);

		m_CommandQueue->WriteToTexture(m_FontTexture, 0, 0, 0, 0, width, height, pixels, bufferSize);

		Graphics::TextureViewDescription viewDesc = {};
		viewDesc.TargetTexture					  = m_FontTexture;
		viewDesc.Format							  = m_FontTexture->GetPixelFormat();
		viewDesc.Range							  = {.BaseMipLevel = 0, .LevelCount = 1, .BaseArrayLayer = 0, .LayerCount = 1};
		viewDesc.DebugName						  = "ImGui Font View";
		m_FontTextureView						  = m_GraphicsDevice->CreateTextureView(viewDesc);

		UnbindTexture(m_FontTextureID);

		m_FontTextureID = BindTexture(m_FontTextureView);
		io.Fonts->SetTexID(m_FontTextureID);
		io.Fonts->ClearTexData();
	}

	ImTextureID ImGuiGraphicsRenderer::BindTexture(Nexus::Ref<Nexus::Graphics::ITextureView> texture)
	{
		ImTextureID id = (ImTextureID)m_TextureID++;

		Ref<Graphics::IResourceSet> resourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);

		ImGuiDescriptorInfo &info = m_Descriptors[id];
		info.m_Texture			  = texture;
		info.m_Sampler			  = m_Sampler;
		info.m_ResourceSet		  = resourceSet;

		Graphics::CombinedImageSampler ciSampler = {};
		ciSampler.ImageTexture					 = texture;
		ciSampler.ImageSampler					 = m_Sampler;
		resourceSet->WriteCombinedImageSampler(ciSampler, "u_Texture");
		resourceSet->Flush();

		return id;
	}

	void ImGuiGraphicsRenderer::UnbindTexture(ImTextureID id)
	{
		if (m_Descriptors.contains(id))
		{
			m_Descriptors.erase(id);
		}
	}

	void ImGuiGraphicsRenderer::BeforeLayout(Nexus::TimeSpan gameTime)
	{
		NX_PROFILE_FUNCTION();

		auto &io	 = ImGui::GetIO();
		io.DeltaTime = (float)gameTime.GetSeconds<float>();

		auto [width, height]	   = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();
		io.DisplaySize			   = {static_cast<float>(width), static_cast<float>(height)};
		io.DisplayFramebufferScale = {1, 1};

		UpdateMonitors();
		UpdateInput();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiGraphicsRenderer::AfterLayout()
	{
		NX_PROFILE_FUNCTION();
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
					ImGuiWindowInfo					*info	   = (ImGuiWindowInfo *)platform_io.Viewports[i]->PlatformUserData;
					Nexus::IWindow					*window	   = info->Window;
					Ref<Nexus::Graphics::ISwapchain> swapchain = info->Swapchain;

					if (window && !window->IsClosing())
					{
						if (swapchain)
						{
							RenderDrawData(platform_io.Viewports[i]->DrawData);
							swapchain->SwapBuffers(Graphics::SwapchainPresentDescription {});
						}
					}
				}
			}
		}

		RenderDrawData(ImGui::GetDrawData());
		UpdateCursor();

		m_GraphicsDevice->WaitForIdle();
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

		SetupInput(Nexus::GetApplication()->GetPrimaryWindow());
		SetupHandlers();
	}

	void ImGuiGraphicsRenderer::AddTextInput(const TextInputEventArgs &args)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.AddInputCharactersUTF8(args.Text.c_str());
	}

	void ImGuiGraphicsRenderer::AddMouseScroll(const MouseScrolledEventArgs &args)
	{
		ImGuiIO &io = ImGui::GetIO();
		auto [x, y] = args.Scroll;
		io.AddMouseWheelEvent(x, y);
	}

	static std::map<Nexus::ScanCode, ImGuiKey> s_KeyMap = {
		{Nexus::ScanCode::Tab, ImGuiKey_Tab},
		{Nexus::ScanCode::Left, ImGuiKey_LeftArrow},
		{Nexus::ScanCode::Right, ImGuiKey_RightArrow},
		{Nexus::ScanCode::Up, ImGuiKey_UpArrow},
		{Nexus::ScanCode::Down, ImGuiKey_DownArrow},
		{Nexus::ScanCode::PageUp, ImGuiKey_PageUp},
		{Nexus::ScanCode::PageDown, ImGuiKey_PageDown},
		{Nexus::ScanCode::Home, ImGuiKey_Home},
		{Nexus::ScanCode::End, ImGuiKey_End},
		{Nexus::ScanCode::Delete, ImGuiKey_Delete},
		{Nexus::ScanCode::Backspace, ImGuiKey_Backspace},
		{Nexus::ScanCode::Return, ImGuiKey_Enter},
		{Nexus::ScanCode::Escape, ImGuiKey_Escape},
		{Nexus::ScanCode::Space, ImGuiKey_Space},
		{Nexus::ScanCode::A, ImGuiKey_A},
		{Nexus::ScanCode::C, ImGuiKey_C},
		{Nexus::ScanCode::V, ImGuiKey_V},
		{Nexus::ScanCode::X, ImGuiKey_X},
		{Nexus::ScanCode::Y, ImGuiKey_Y},
		{Nexus::ScanCode::Z, ImGuiKey_Z},
	};

	void ImGuiGraphicsRenderer::AddKeyPressed(const KeyPressedEventArgs &args)
	{
		auto &io = ImGui::GetIO();

		if (args.ScanCode == ScanCode::LeftShift || args.ScanCode == ScanCode::RightShift)
		{
			io.KeyShift = true;
		}

		if (args.ScanCode == ScanCode::LeftControl || args.ScanCode == ScanCode::RightControl)
		{
			io.KeyCtrl = true;
		}

		if (args.ScanCode == ScanCode::LeftAlt || args.ScanCode == ScanCode::RightAlt)
		{
			io.KeyAlt = true;
		}

		if (args.ScanCode == ScanCode::LeftGUI || args.ScanCode == ScanCode::RightGUI)
		{
			io.KeySuper = true;
		}

		if (s_KeyMap.contains(args.ScanCode))
		{
			io.AddKeyEvent(s_KeyMap[args.ScanCode], true);
		}
	}

	void ImGuiGraphicsRenderer::AddKeyReleased(const KeyReleasedEventArgs &args)
	{
		auto &io = ImGui::GetIO();

		if (args.ScanCode == ScanCode::LeftShift || args.ScanCode == ScanCode::RightShift)
		{
			io.KeyShift = false;
		}

		if (args.ScanCode == ScanCode::LeftControl || args.ScanCode == ScanCode::RightControl)
		{
			io.KeyCtrl = false;
		}

		if (args.ScanCode == ScanCode::LeftAlt || args.ScanCode == ScanCode::RightAlt)
		{
			io.KeyAlt = false;
		}

		if (args.ScanCode == ScanCode::LeftGUI || args.ScanCode == ScanCode::RightGUI)
		{
			io.KeySuper = false;
		}

		if (s_KeyMap.contains(args.ScanCode))
		{
			io.AddKeyEvent(s_KeyMap[args.ScanCode], false);
		}
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
		/*io.AddKeyEvent(ImGuiKey_Tab, activeWindow->IsKeyDown(Nexus::ScanCode::Tab));
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
		io.KeySuper = activeWindow->IsKeyDown(ScanCode::LeftGUI) || activeWindow->IsKeyDown(ScanCode::RightGUI);*/

		MouseState state	= Platform::GetFocussedMouseState();
		auto	   mousePos = activeWindow->GetMousePosition();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			state	 = Platform::GetGlobalMouseState();
			mousePos = state.MousePosition;
		}

		if (mousePos)
		{
			auto [mouseX, mouseY] = mousePos.value();
			io.AddMousePosEvent(mouseX, mouseY);
		}

		io.AddMouseButtonEvent(0, state.LeftButton == MouseButtonState::Pressed);
		io.AddMouseButtonEvent(1, state.RightButton == MouseButtonState::Pressed);
		io.AddMouseButtonEvent(2, state.MiddleButton == MouseButtonState::Pressed);
	}

	void ImGuiGraphicsRenderer::RenderDrawData(ImDrawData *drawData)
	{
		NX_PROFILE_FUNCTION();

		drawData->ScaleClipRects(ImGui::GetIO().DisplayFramebufferScale);
		UpdateBuffers(drawData);
		RenderCommandLists(drawData);
	}

	void ImGuiGraphicsRenderer::UpdateBuffers(ImDrawData *drawData)
	{
		NX_PROFILE_FUNCTION();

		if (drawData->TotalVtxCount == 0)
		{
			return;
		}

		if (drawData->TotalVtxCount > m_VertexBufferCount)
		{
			m_VertexBufferCount = drawData->TotalVtxCount * 1.5f;

			Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
			vertexBufferDesc.Access									  = Graphics::BufferMemoryAccess::Upload;
			vertexBufferDesc.Usage									  = Graphics::BufferUsage_Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(ImDrawVert);
			vertexBufferDesc.SizeInBytes							  = m_VertexBufferCount * sizeof(ImDrawVert);
			vertexBufferDesc.DebugName								  = "ImGui Vertex Buffer";
			m_VertexBuffer											  = m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc);
		}

		if (drawData->TotalIdxCount > m_IndexBufferCount)
		{
			m_IndexBufferCount = drawData->TotalIdxCount * 1.5f;

			Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
			indexBufferDesc.Access									 = Graphics::BufferMemoryAccess::Upload;
			indexBufferDesc.Usage									 = Graphics::BufferUsage_Index;
			indexBufferDesc.StrideInBytes							 = sizeof(ImDrawIdx);
			indexBufferDesc.SizeInBytes								 = m_IndexBufferCount * sizeof(ImDrawIdx);
			indexBufferDesc.DebugName								 = "ImGui Index Buffer";
			m_IndexBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc);
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
		NX_PROFILE_FUNCTION();

		if (drawData->TotalVtxCount == 0)
			return;

		if (!m_VertexBuffer)
			return;

		if (!m_IndexBuffer)
			return;

		ImGuiWindowInfo *info = (ImGuiWindowInfo *)drawData->OwnerViewport->PlatformUserData;

		m_CommandList->Begin();
		m_CommandList->BeginDebugGroup("Rendering ImGui");

		ImVec2 pos = drawData->DisplayPos;

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
					m_CommandList->SetPipeline(m_Pipeline);

					Ref<Graphics::ISwapchain> swapchain = info->Swapchain;
					m_CommandList->SetFramebuffer(swapchain->GetCurrentFramebuffer());

					Graphics::VertexBufferView vertexBufferView = {};
					vertexBufferView.BufferHandle				= m_VertexBuffer;
					vertexBufferView.Offset						= 0;
					vertexBufferView.Size						= m_VertexBuffer->GetSizeInBytes();
					m_CommandList->SetVertexBuffer(vertexBufferView, 0);

					Graphics::IndexBufferView indexBufferView = {};
					indexBufferView.BufferHandle			  = m_IndexBuffer;
					indexBufferView.Offset					  = 0;
					indexBufferView.Size					  = m_IndexBuffer->GetSizeInBytes();
					indexBufferView.BufferFormat			  = Graphics::IndexFormat::UInt16;
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

					auto	 &io  = ImGui::GetIO();
					glm::mat4 mvp = glm::ortho<float>(pos.x, pos.x + drawData->DisplaySize.x, pos.y + drawData->DisplaySize.y, pos.y, -1.f, 1.0f);

					auto &descriptorInfo = m_Descriptors.at(drawCmd.TextureId);

					Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
					resourceBindingDesc.TargetResourceSet							   = descriptorInfo.m_ResourceSet;
					resourceBindingDesc.DynamicOffsets								   = {};
					m_CommandList->SetResourceSet(resourceBindingDesc);

					m_CommandList->WritePushConstants("PushConstants", &mvp, sizeof(mvp), 0);

					Graphics::DrawIndexedDescription drawDesc = {};
					drawDesc.VertexStart					  = drawCmd.VtxOffset + vtxOffset;
					drawDesc.IndexStart						  = drawCmd.IdxOffset + idxOffset;
					drawDesc.InstanceStart					  = 0;
					drawDesc.IndexCount						  = drawCmd.ElemCount;
					drawDesc.InstanceCount					  = 1;
					m_CommandList->DrawIndexed(drawDesc);
				}
			}

			idxOffset += cmdList->IdxBuffer.Size;
			vtxOffset += cmdList->VtxBuffer.Size;
		}

		m_CommandList->EndDebugGroup();
		m_CommandList->End();

		m_CommandQueue->SubmitCommandList(m_CommandList);
	}

	void ImGuiGraphicsRenderer::UpdateCursor()
	{
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();

		if (cursor == m_PreviousCursor)
		{
			return;
		}

		auto window		 = Nexus::GetApplication()->GetPrimaryWindow();
		m_PreviousCursor = cursor;

		switch (cursor)
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
			auto [monitorX, monitorY]				   = monitor.Position;
			auto [monitorWidth, monitorHeight]		   = monitor.Size;
			auto [monitorWorkX, monitorWorkY]		   = monitor.WorkPosition;
			auto [monitorWorkWidth, monitorWorkHeight] = monitor.WorkSize;

			ImGuiPlatformMonitor m;
			m.DpiScale = monitor.DPI;
			m.MainPos  = {static_cast<float>(monitorX), static_cast<float>(monitorY)};
			m.MainSize = {static_cast<float>(monitorWidth), static_cast<float>(monitorHeight)};
			m.WorkPos  = {static_cast<float>(monitorWorkX), static_cast<float>(monitorWorkY)};
			m.WorkSize = {static_cast<float>(monitorWorkWidth), static_cast<float>(monitorWorkHeight)};

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

			WindowDescription windowSpec;
			windowSpec.Width  = vp->Size.x;
			windowSpec.Height = vp->Size.y;
			windowSpec.Flags  = WindowFlags_HighPixelDensity | WindowFlags_Borderless;

			Nexus::Graphics::SwapchainDescription swapchainSpec = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription();

			Nexus::IWindow *window = Platform::CreatePlatformWindow(windowSpec);
			window->SetWindowPosition(vp->Pos.x, vp->Pos.y);

			swapchainSpec.Width	  = windowSpec.Width;
			swapchainSpec.Height  = windowSpec.Height;
			swapchainSpec.Surface = Utils::CreateSurfaceForWindow(Nexus::GetApplication()->GetGraphicsDevice(), window);

			Ref<Nexus::Graphics::ISwapchain> swapchain = app->GetGraphicsCommandQueue()->CreateSwapchain(swapchainSpec);

			ImGuiWindowInfo *info = new ImGuiWindowInfo();
			info->Window		  = window;
			info->Swapchain		  = swapchain;

			SetupInput(Nexus::GetApplication()->GetPrimaryWindow());

			vp->PlatformUserData = info;
			vp->RendererUserData = info;
		};

		platformIo.Platform_DestroyWindow = [](ImGuiViewport *vp) -> void
		{
			if (vp->PlatformUserData && vp->RendererUserData)
			{
				ImGuiWindowInfo *info = (ImGuiWindowInfo *)vp->PlatformUserData;

				if (info->Window != Nexus::GetApplication()->GetPrimaryWindow())
				{
					info->Window->Close();
				}
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
					uint32_t width	= static_cast<uint32_t>(size.x);
					uint32_t height = static_cast<uint32_t>(size.y);

					info->Window->SetSize(width, height);
					info->Swapchain->Resize(width, height);
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
					auto [x, y] = info->Window->GetWindowPosition();
					return {static_cast<float>(x), static_cast<float>(y)};
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
					auto [width, height] = info->Window->GetWindowSize();
					return {static_cast<float>(width), static_cast<float>(height)};
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