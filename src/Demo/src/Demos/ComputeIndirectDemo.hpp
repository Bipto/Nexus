#pragma once

#include "Demo.hpp"

namespace Demos
{
	class ComputeIndirectDemo : public Demo
	{
	  public:
		ComputeIndirectDemo(const std::string						 &name,
							Nexus::Application						 *app,
							Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
							Nexus::Graphics::CommandQueueHandle		  commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~ComputeIndirectDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();

			Nexus::Graphics::TextureDescription textureDesc = {};
			textureDesc.Width								= 512;
			textureDesc.Height								= 512;
			textureDesc.Format								= Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
			textureDesc.Samples								= 1;
			textureDesc.MipLevels							= 1;
			textureDesc.Usage								= Nexus::Graphics::TextureUsage_Storage | Nexus::Graphics::TextureUsage_Sampled;
			textureDesc.DebugName							= "Compute Texture";
			m_Texture										= m_GraphicsDevice->CreateTexture(textureDesc);

			Nexus::Graphics::TextureViewDescription viewDesc = {};
			viewDesc.TargetTexture							 = m_Texture;
			viewDesc.Format									 = m_Texture->GetPixelFormat();
			viewDesc.Range									 = {.BaseMipLevel = 0, .LevelCount = 1, .BaseArrayLayer = 0, .LayerCount = 1};
			viewDesc.DebugName								 = "Compute Texture View";
			m_TextureView									 = m_GraphicsDevice->CreateTextureView(viewDesc);

			Nexus::Graphics::ComputePipelineDescription desc = {};
			desc.ComputeShader								 = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																					  "resources/demo/shaders/compute/compute.glsl",
																					  Nexus::GetApplication()->GetApplicationPath(),
																					  Nexus::Graphics::ShaderStage::Compute);
			desc.ResourceDescription.Descriptors			 = {
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_Image",
																 .Type				 = Nexus::Graphics::ResourceDescriptorType::StorageImage,
																 .CountOrSizeInBytes = 1},
			};
			m_ComputePipeline = m_GraphicsDevice->CreateComputePipeline(desc);

			m_ResourceSet		  = m_GraphicsDevice->CreateResourceSet(m_ComputePipeline);
			m_ImGuiTextureBinding = m_ImGuiRenderer->BindTexture(m_TextureView);

			Nexus::Graphics::StorageImageView storageImageView = {};
			storageImageView.Texture						   = m_Texture;
			storageImageView.MipLevel						   = 0;
			storageImageView.Access							   = Nexus::Graphics::ShaderAccess::ReadWrite;
			m_ResourceSet->WriteStorageImage(storageImageView, "u_Image");

			m_ResourceSet->Flush();

			Nexus::Graphics::DeviceBufferDescription indirectDesc = {};
			indirectDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
			indirectDesc.Usage									  = Nexus::Graphics::BufferUsage_Indirect;
			indirectDesc.SizeInBytes							  = sizeof(Nexus::Graphics::IndirectDispatchArguments);
			indirectDesc.StrideInBytes							  = sizeof(Nexus::Graphics::IndirectDispatchArguments);
			m_IndirectBuffer									  = m_GraphicsDevice->CreateDeviceBuffer(indirectDesc);

			Nexus::Graphics::IndirectDispatchArguments args = {};
			args.GroupCountX								= m_Texture->GetDescription().Width;
			args.GroupCountY								= m_Texture->GetDescription().Height;
			args.GroupCountZ								= 1;
			m_IndirectBuffer->SetData(&args, 0, sizeof(args));
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			auto [width, height] = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();

			m_CommandList->Begin();

			m_CommandList->SetPipeline(m_ComputePipeline);
			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = m_ResourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			Nexus::Graphics::DispatchIndirectDescription dispatchDesc = {};
			dispatchDesc.IndirectBuffer								  = m_IndirectBuffer.get();
			dispatchDesc.Offset										  = 0;
			dispatchDesc.Stride										  = m_IndirectBuffer->GetStrideInBytes();
			m_CommandList->DispatchIndirect(dispatchDesc);

			Nexus::Graphics::SwapchainHandle   swapchain   = Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Graphics::FramebufferHandle framebuffer = swapchain->GetCurrentFramebuffer();
			m_CommandList->SetFramebuffer(framebuffer);

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= width;
			vp.Height	= height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			m_CommandList->SetViewport(vp);

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = width;
			scissor.Height = height;
			m_CommandList->SetScissor(scissor);

			m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
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
			return "Updating a texture using a Compute Pipeline and an indirect buffer";
		}

	  private:
		Nexus::Graphics::CommandListHandle		   m_CommandList		 = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_IndirectBuffer		 = nullptr;
		Nexus::Graphics::PipelineHandle			   m_ComputePipeline	 = {};
		Nexus::Graphics::ResourceSetHandle		   m_ResourceSet		 = {};
		Nexus::Graphics::TextureHandle			   m_Texture			 = {};
		Nexus::Graphics::TextureViewHandle		   m_TextureView		 = {};
		glm::vec3								   m_ClearColour		 = {0.7f, 0.2f, 0.3f};
		ImTextureID								   m_ImGuiTextureBinding = 0;
	};
}	 // namespace Demos