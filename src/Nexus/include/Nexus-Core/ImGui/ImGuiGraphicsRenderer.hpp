
#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"

inline void *ImGuiAlloc(size_t size, void *user_data)
{
	return malloc(size);
}

inline void ImGuiFree(void *ptr, void *user_data)
{
	if (ptr)
	{
		free(ptr);
	}
}

namespace Nexus::ImGuiUtils
{
	struct ImGuiWindowInfo
	{
		Nexus::IWindow					*Window				 = nullptr;
		Ref<Nexus::Graphics::ISwapchain> Swapchain			 = nullptr;
		uint32_t						 LastSwapchainWidth	 = 0;
		uint32_t						 LastSwapchainHeight = 0;
		uint32_t						 LastWindowWidth	 = 0;
		uint32_t						 LastWindowHeight	 = 0;
	};

	struct ImGuiDescriptorInfo
	{
		Ref<Graphics::ITextureView> m_Texture	  = nullptr;
		Ref<Graphics::IResourceSet> m_ResourceSet = nullptr;
		Ref<Graphics::ISampler>		m_Sampler	  = nullptr;
	};

	class NX_API ImGuiGraphicsRenderer
	{
	  public:
		ImGuiGraphicsRenderer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
		virtual ~ImGuiGraphicsRenderer();
		void RebuildFontAtlas();

		ImTextureID BindTexture(Nexus::Ref<Nexus::Graphics::ITextureView> texture);
		void		UnbindTexture(ImTextureID id);

		void BeforeLayout(Nexus::TimeSpan gameTime);
		void AfterLayout();

		ImGuiIO		 &GetIO();
		ImGuiContext *GetContext();
		void		  SetContext(ImGuiContext *context);

		void AddTextInput(const TextInputEventArgs &args);
		void AddMouseScroll(const MouseScrolledEventArgs &args);
		void AddKeyPressed(const KeyPressedEventArgs &args);
		void AddKeyReleased(const KeyReleasedEventArgs &args);
		void AddMouseMoved(const MouseMovedEventArgs &args);
		void AddMouseButtonPressed(const MouseButtonPressedEventArgs &args);
		void AddMouseButtonReleased(const MouseButtonReleasedEventArgs &args);

		static ImGuiGraphicsRenderer *GetCurrentRenderer();
		static void					  SetCurrentRenderer(ImGuiGraphicsRenderer *renderer);

	  private:
		void CreatePipeline();
		void UpdateInput();
		void RenderDrawData(ImDrawData *drawData);
		void UpdateBuffers(ImDrawData *drawData);
		void RenderCommandLists(ImDrawData *drawData);
		void UpdateCursor();
		void UpdateMonitors();
		void SetupHandlers();

	  private:
		Nexus::Application							  *m_Application	 = nullptr;
		Nexus::Graphics::IGraphicsDevice			  *m_GraphicsDevice	 = nullptr;
		Nexus::Ref<Nexus::Graphics::ICommandQueue>	   m_CommandQueue	 = nullptr;
		Nexus::Ref<Nexus::Graphics::ICommandList>	   m_CommandList	 = nullptr;
		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline> m_Pipeline		 = nullptr;
		Nexus::Ref<Nexus::Graphics::ITexture>		   m_FontTexture	 = nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView>	   m_FontTextureView = nullptr;

		ImGuiContext *m_Context = nullptr;

		std::map<ImTextureID, ImGuiDescriptorInfo> m_Descriptors = {};

		Nexus::Ref<Nexus::Graphics::IShaderModule> m_VertexShader	= nullptr;
		Nexus::Ref<Nexus::Graphics::IShaderModule> m_FragmentShader = nullptr;

		Nexus::Ref<Nexus::Graphics::ISampler> m_Sampler		  = nullptr;
		uint64_t							  m_TextureID	  = 0;
		ImTextureID							  m_FontTextureID = 0;

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_VertexBuffer	   = nullptr;
		uint32_t								   m_VertexBufferCount = 0;

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_IndexBuffer	  = nullptr;
		uint32_t								   m_IndexBufferCount = 0;

		std::vector<int> m_Keys;
		ImGuiMouseCursor m_PreviousCursor = ImGuiMouseCursor_Arrow;
	};
}	 // namespace Nexus::ImGuiUtils