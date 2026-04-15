#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Graphics/HdriProcessor.hpp"
#include "stb_image.h"

namespace Demos
{

	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_CUBEMAP
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};

	class CubemapDemo : public Demo
	{
	  public:
		CubemapDemo(const std::string						  &name,
					Nexus::Application						  *app,
					Nexus::ImGuiUtils::ImGuiGraphicsRenderer  *imGuiRenderer,
					Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
			auto [width, height] = m_Window->GetWindowSizeInPixels();
			m_Camera			 = Nexus::FirstPersonCamera(m_GraphicsDevice, width, height, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		virtual ~CubemapDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();

			CreatePipeline();

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			cameraUniformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_CUBEMAP);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_CUBEMAP);
			m_CameraUniformBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc);

			Nexus::Graphics::SamplerDescription samplerDesc {};
			samplerDesc.AddressModeU = Nexus::Graphics::SamplerAddressMode::Clamp;
			samplerDesc.AddressModeV = Nexus::Graphics::SamplerAddressMode::Clamp;
			samplerDesc.AddressModeW = Nexus::Graphics::SamplerAddressMode::Clamp;
			samplerDesc.SampleFilter = Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint;
			m_Sampler				 = m_GraphicsDevice->CreateSampler(samplerDesc);

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
			m_Cube = factory.CreateCube();

			Nexus::Graphics::HdriProcessor processor(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/hdri/hangar_interior_4k.hdr"),
													 m_GraphicsDevice,
													 m_CommandQueue);
			m_Cubemap = processor.Generate(2048);

			Nexus::Graphics::TextureViewDescription viewDesc = {};
			viewDesc.TargetTexture							 = m_Cubemap;
			viewDesc.Format									 = m_Cubemap->GetPixelFormat();
			viewDesc.Range									 = {.BaseMipLevel	= 0,
																.LevelCount		= m_Cubemap->GetMipLevels(),
																.BaseArrayLayer = 0,
																.LayerCount		= m_Cubemap->GetDepthOrArrayLayers()};
			viewDesc.DebugName								 = "Cubemap View";
			m_CubemapView									 = m_GraphicsDevice->CreateTextureView(viewDesc);

			Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
			cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer;
			cameraUniformBufferView.Offset							   = 0;
			cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
			m_ResourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

			Nexus::Graphics::CombinedImageSampler ciSampler = {};
			ciSampler.ImageTexture							= m_CubemapView;
			ciSampler.ImageSampler							= m_Sampler;
			m_ResourceSet->WriteCombinedImageSampler(ciSampler, "u_Skybox");

			m_ResourceSet->Flush();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			auto [width, height] = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();
			m_Camera.Update(width, height, time);

			// handle camera movement
			if (m_CameraActive)
			{
				Nexus::IWindow *window			 = Nexus::GetApplication()->GetPrimaryWindow();
				auto [windowWidth, windowHeight] = window->GetWindowSize();
				window->WarpMouse(static_cast<float>(windowWidth) / 2.0f, static_cast<float>(windowHeight) / 2.0f);

				glm::vec3 movement	  = {0.0f, 0.0f, 0.0f};
				float	  cameraSpeed = 2.0f * time.GetSeconds<float>();

				if (Nexus::Input::IsKeyDown(Nexus::ScanCode::LeftShift) || Nexus::Input::IsKeyDown(Nexus::ScanCode::RightShift))
				{
					cameraSpeed *= 2.0f;
				}

				if (Nexus::Input::IsKeyDown(Nexus::ScanCode::W))
				{
					movement.z += cameraSpeed;
				}

				if (Nexus::Input::IsKeyDown(Nexus::ScanCode::S))
				{
					movement.z -= cameraSpeed;
				}

				if (Nexus::Input::IsKeyDown(Nexus::ScanCode::A))
				{
					movement.x += cameraSpeed;
				}

				if (Nexus::Input::IsKeyDown(Nexus::ScanCode::D))
				{
					movement.x -= cameraSpeed;
				}

				m_Camera.Translate(movement);
			}

			m_CameraUniforms.Projection = m_Camera.GetProjection();
			m_CameraUniforms.View		= glm::mat4(glm::mat3(m_Camera.GetView()));
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);

			Nexus::Ref<Nexus::Graphics::ISwapchain> swapchain	= Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Graphics::FramebufferHandle		framebuffer = swapchain->GetCurrentFramebuffer();
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

			Nexus::Graphics::ClearDepthStencilValue clearDepth {};
			m_CommandList->ClearDepthTarget(clearDepth);

			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = m_ResourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_Cube->GetVertexBuffer();
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Size							   = m_Cube->GetVertexBuffer()->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_Cube->GetIndexBuffer();
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_Cube->GetIndexBuffer()->GetSizeInBytes();
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			auto indexCount = m_Cube->GetIndexBuffer()->GetCount();

			Nexus::Graphics::DrawIndexedDescription drawDesc = {};
			drawDesc.VertexStart							 = 0;
			drawDesc.IndexStart								 = 0;
			drawDesc.InstanceStart							 = 0;
			drawDesc.IndexCount								 = indexCount;
			drawDesc.InstanceCount							 = 1;
			m_CommandList->DrawIndexed(drawDesc);

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
			auto [width, height] = m_Window->GetWindowSize();
			m_Camera.Update(width, height, time);
		}

		virtual void RenderUI() override
		{
		}

		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.VertexModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																								  "resources/demo/shaders/cubemap/cubemap.vert.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																   "resources/demo/shaders/cubemap/cubemap.frag.glsl",
																   Nexus::GetApplication()->GetApplicationPath(),
																   Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ColourTargetCount = 1;
			pipelineDescription.ColourFormats[0]  = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.Samples			  = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			pipelineDescription.ResourceDescription.Descriptors = {
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_Skybox",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "Camera",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
													 .CountOrSizeInBytes = 1}};

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

		virtual std::string GetInfo() const override
		{
			return "Creating and rendering a cubemap to provide an environment.";
		}

		virtual void OnEvent(const Nexus::Event &event) override
		{
			Nexus::EventDispatcher dispatcher = {};

			dispatcher.Subscribe<Nexus::MouseButtonPressedEventArgs>(
				[this](const Nexus::MouseButtonPressedEventArgs &args)
				{
					if (args.Button == Nexus::MouseButton::Right)
					{
						m_CameraActive = true;
						Nexus::GetApplication()->GetPrimaryWindow()->SetRelativeMouseMode(true);
					}
				});

			dispatcher.Subscribe<Nexus::KeyPressedEventArgs>(
				[this](const Nexus::KeyPressedEventArgs &args)
				{
					if (args.ScanCode == Nexus::ScanCode::Escape)
					{
						m_CameraActive = false;
						Nexus::GetApplication()->GetPrimaryWindow()->SetRelativeMouseMode(false);
					}
				});

			dispatcher.Subscribe<Nexus::MouseMovedEventArgs>(
				[this](const Nexus::MouseMovedEventArgs &args)
				{
					if (m_CameraActive)
					{
						m_Camera.Rotate(args.Movement.first, args.Movement.second);
					}
				});

			dispatcher.Dispatch(event);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::ICommandList> m_CommandList = nullptr;
		Nexus::Graphics::TextureHandle			  m_Cubemap		= {};
		Nexus::Graphics::TextureViewHandle		  m_CubemapView = {};
		Nexus::Graphics::SamplerHandle			  m_Sampler		= {};
		glm::vec3								  m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline> m_Pipeline	 = nullptr;
		Nexus::Ref<Nexus::Graphics::IResourceSet>	   m_ResourceSet = nullptr;

		Nexus::Ref<Nexus::Graphics::Mesh> m_Cube = nullptr;

		VB_UNIFORM_CAMERA_DEMO_CAMERA			   m_CameraUniforms		 = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_CameraUniformBuffer = nullptr;

		Nexus::FirstPersonCamera m_Camera		= {};
		bool					 m_CameraActive = false;
	};
}	 // namespace Demos