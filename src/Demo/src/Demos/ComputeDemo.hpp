#pragma once

#include "Demo.hpp"

namespace Demos
{
	class ComputeDemo : public Demo
	{
	  public:
		ComputeDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~ComputeDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			Nexus::Graphics::TextureDescription textureSpec	  = {};
			textureSpec.Width								  = 512;
			textureSpec.Height								  = 512;
			textureSpec.Format								  = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
			textureSpec.Samples								  = 1;
			textureSpec.MipLevels							  = 1;
			textureSpec.Usage								  = Nexus::Graphics::TextureUsage_Storage | Nexus::Graphics::TextureUsage_Sampled;
			m_Texture										  = Nexus::Ref<Nexus::Graphics::Texture>(m_GraphicsDevice->CreateTexture(textureSpec));

			Nexus::Graphics::ComputePipelineDescription desc = {};
			desc.ComputeShader =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/compute.glsl", Nexus::Graphics::ShaderStage::Compute);
			m_ComputePipeline				   = m_GraphicsDevice->CreateComputePipeline(desc);

			m_ResourceSet		  = m_GraphicsDevice->CreateResourceSet(m_ComputePipeline);
			m_ImGuiTextureBinding = m_ImGuiRenderer->BindTexture(m_Texture);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			Nexus::Graphics::StorageImageView storageImageView = {};
			storageImageView.TextureHandle					   = m_Texture;
			storageImageView.MipLevel						   = 0;
			storageImageView.Access							   = Nexus::Graphics::ShaderAccess::ReadWrite;
			m_ResourceSet->WriteStorageImage(storageImageView, "out_tex");

			m_CommandList->Begin();

			m_CommandList->SetPipeline(m_ComputePipeline);
			m_CommandList->SetResourceSet(m_ResourceSet);

			Nexus::Graphics::DispatchDescription dispatchDesc = {};
			dispatchDesc.WorkGroupCountX					  = m_Texture->GetDescription().Width;
			dispatchDesc.WorkGroupCountY					  = m_Texture->GetDescription().Height;
			dispatchDesc.WorkGroupCountZ					  = 1;
			m_CommandList->Dispatch(dispatchDesc);

			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(Nexus::GetApplication()->GetPrimarySwapchain()));

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().X;
			vp.Height	= Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().Y;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			m_CommandList->SetViewport(vp);

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().X;
			scissor.Height = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().Y;
			m_CommandList->SetScissor(scissor);

			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size) override
		{
		}

		virtual void RenderUI() override
		{
			ImGui::Image(m_ImGuiTextureBinding, ImVec2(512, 512));
		}

		virtual std::string GetInfo() const override
		{
			return "Updating a texture using a Compute Pipeline";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList>	 m_CommandList;
		Nexus::Ref<Nexus::Graphics::ComputePipeline> m_ComputePipeline;
		Nexus::Ref<Nexus::Graphics::ResourceSet>	 m_ResourceSet;
		Nexus::Ref<Nexus::Graphics::Texture>		 m_Texture;
		glm::vec3									 m_ClearColour		   = {0.7f, 0.2f, 0.3f};
		ImTextureID									 m_ImGuiTextureBinding = 0;
	};
}	 // namespace Demos