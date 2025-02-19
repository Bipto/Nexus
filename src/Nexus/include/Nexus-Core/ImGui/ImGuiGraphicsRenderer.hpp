#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"

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

	class ImGuiGraphicsRenderer
	{
	  public:
		NX_API ImGuiGraphicsRenderer(Nexus::Application *app);
		NX_API virtual ~ImGuiGraphicsRenderer();
		NX_API void RebuildFontAtlas();

		NX_API ImTextureID BindTexture(Nexus::Ref<Nexus::Graphics::Texture2D> texture);
		NX_API void		   UnbindTexture(ImTextureID id);

		NX_API void BeforeLayout(Nexus::TimeSpan gameTime);
		NX_API void AfterLayout();

		NX_API ImGuiIO		&GetIO();
		NX_API ImGuiContext *GetContext();

		static ImGuiGraphicsRenderer *GetCurrentRenderer();

	  private:
		void CreateTextPipeline();
		void CreateImagePipeline();
		void SetupInput();
		void UpdateInput();
		void RenderDrawData(ImDrawData *drawData);
		void UpdateBuffers(ImDrawData *drawData);
		void RenderCommandLists(ImDrawData *drawData);
		void UpdateCursor();
		void UpdateMonitors();

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