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
		Nexus::IWindow				   *Window	  = nullptr;
		Ref<Nexus::Graphics::Swapchain> Swapchain = nullptr;
	};

	struct ImGuiDescriptorInfo
	{
		Ref<Graphics::Texture>		m_Texture		= nullptr;
		Ref<Graphics::ResourceSet>	m_ResourceSet	= nullptr;
		Ref<Graphics::Sampler>		m_Sampler		= nullptr;
		Ref<Graphics::DeviceBuffer> m_UniformBuffer = nullptr;
	};

	class NX_API ImGuiGraphicsRenderer
	{
	  public:
		ImGuiGraphicsRenderer(Nexus::Application *app, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
		virtual ~ImGuiGraphicsRenderer();
		void RebuildFontAtlas();

		ImTextureID BindTexture(Nexus::Ref<Nexus::Graphics::Texture> texture);
		void		UnbindTexture(ImTextureID id);

		void BeforeLayout(Nexus::TimeSpan gameTime);
		void AfterLayout();

		ImGuiIO		 &GetIO();
		ImGuiContext *GetContext();
		void		  SetContext(ImGuiContext *context);

		static ImGuiGraphicsRenderer *GetCurrentRenderer();
		static void					  SetCurrentRenderer(ImGuiGraphicsRenderer *renderer);

	  private:
		void		CreatePipeline();
		static void SetupInput(IWindow *window);
		void		UpdateInput();
		void		RenderDrawData(ImDrawData *drawData);
		void		UpdateBuffers(ImDrawData *drawData);
		void		RenderCommandLists(ImDrawData *drawData);
		void		UpdateCursor();
		void		UpdateMonitors();
		void		SetupHandlers();

	  private:
		Nexus::Application							 *m_Application	   = nullptr;
		Nexus::Graphics::GraphicsDevice				 *m_GraphicsDevice = nullptr;
		Nexus::Ref<Nexus::Graphics::ICommandQueue>	  m_CommandQueue   = nullptr;
		Nexus::Ref<Nexus::Graphics::CommandList>	  m_CommandList	   = nullptr;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline	   = nullptr;
		Nexus::Ref<Nexus::Graphics::Texture>		  m_FontTexture	   = nullptr;

		ImGuiContext *m_Context = nullptr;

		std::map<ImTextureID, ImGuiDescriptorInfo> m_Descriptors = {};

		Nexus::Ref<Nexus::Graphics::ShaderModule> m_VertexShader   = nullptr;
		Nexus::Ref<Nexus::Graphics::ShaderModule> m_FragmentShader = nullptr;

		Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler		 = nullptr;
		uint64_t							 m_TextureID	 = 0;
		ImTextureID							 m_FontTextureID = 0;

		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_VertexBuffer	  = nullptr;
		uint32_t								  m_VertexBufferCount = 0;

		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_IndexBuffer		 = nullptr;
		uint32_t								  m_IndexBufferCount = 0;

		std::vector<int> m_Keys;
	};
}	 // namespace Nexus::ImGuiUtils