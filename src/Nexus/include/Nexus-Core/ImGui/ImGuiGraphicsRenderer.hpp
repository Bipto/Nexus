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
		Nexus::IWindow			   *Window	  = nullptr;
		Nexus::Graphics::Swapchain *Swapchain = nullptr;

		virtual ~ImGuiWindowInfo()
		{
			delete Swapchain;
			Window->Close();
		}
	};

	class NX_API ImGuiGraphicsRenderer
	{
	  public:
		ImGuiGraphicsRenderer(Nexus::Application *app);
		virtual ~ImGuiGraphicsRenderer();
		void RebuildFontAtlas();

		ImTextureID BindTexture(Nexus::Ref<Nexus::Graphics::Texture2D> texture);
		void		UnbindTexture(ImTextureID id);

		void BeforeLayout(Nexus::TimeSpan gameTime);
		void AfterLayout();

		ImGuiIO		 &GetIO();
		ImGuiContext *GetContext();
		void		  SetContext(ImGuiContext *context);

		static ImGuiGraphicsRenderer *GetCurrentRenderer();
		static void					  SetCurrentRenderer(ImGuiGraphicsRenderer *renderer);

	  private:
		void CreateTextPipeline();
		void CreateImagePipeline();
		static void SetupInput(IWindow *window);
		void UpdateInput();
		void RenderDrawData(ImDrawData *drawData);
		void UpdateBuffers(ImDrawData *drawData);
		void RenderCommandLists(ImDrawData *drawData);
		void UpdateCursor();
		void UpdateMonitors();
		void SetupHandlers();

	  private:
		Nexus::Application						*m_Application	  = nullptr;
		Nexus::Graphics::GraphicsDevice			*m_GraphicsDevice = nullptr;
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList	  = nullptr;
		Nexus::Ref<Nexus::Graphics::Pipeline>	 m_TextPipeline	  = nullptr;
		Nexus::Ref<Nexus::Graphics::Pipeline>	 m_ImagePipeline  = nullptr;
		Nexus::Ref<Nexus::Graphics::Texture2D>	 m_FontTexture	  = nullptr;

		ImGuiContext *m_Context = nullptr;

		Nexus::Ref<Nexus::Graphics::ShaderModule> m_VertexShader   = nullptr;
		Nexus::Ref<Nexus::Graphics::ShaderModule> m_FragmentShader = nullptr;

		std::map<ImTextureID, Nexus::Ref<Nexus::Graphics::Texture2D>>	m_Textures;
		std::map<ImTextureID, Nexus::Ref<Nexus::Graphics::ResourceSet>> m_ResourceSets;

		Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler		 = nullptr;
		uint64_t							 m_TextureID	 = 0;
		ImTextureID							 m_FontTextureID = 0;

		Nexus::Ref<Nexus::Graphics::VertexBuffer> m_VertexBuffer	  = nullptr;
		uint32_t								  m_VertexBufferCount = 0;

		Nexus::Ref<Nexus::Graphics::IndexBuffer> m_IndexBuffer		= nullptr;
		uint32_t								 m_IndexBufferCount = 0;

		Nexus::Ref<Nexus::Graphics::UniformBuffer> m_UniformBuffer = nullptr;

		std::vector<int> m_Keys;
	};
}	 // namespace Nexus::ImGuiUtils