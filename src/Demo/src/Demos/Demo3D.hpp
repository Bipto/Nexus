#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_3D
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};

	struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_3D
	{
		glm::mat4 Transform;
	};

	class Demo3D : public Demo
	{
	  public:
		Demo3D(const std::string						&name,
			   Nexus::Application						*app,
			   Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
			   Nexus::Graphics::CommandQueueHandle		 commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~Demo3D()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();
			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
			m_Mesh						= factory.CreateCube();
			auto [texture, textureView] = Nexus::Utils::CreateTexture2DWithView(
				m_CommandQueue,
				Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg").c_str(),
				true);

			m_Texture	  = texture;
			m_TextureView = textureView;

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			cameraUniformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_3D);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_3D);
			m_CameraUniformBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc);

			Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
			transformUniformBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			transformUniformBufferDesc.Usage									= Nexus::Graphics::BufferUsage_Uniform;
			transformUniformBufferDesc.StrideInBytes							= sizeof(VB_UNIFORM_TRANSFORM_DEMO_3D);
			transformUniformBufferDesc.SizeInBytes								= sizeof(VB_UNIFORM_TRANSFORM_DEMO_3D);
			m_TransformUniformBuffer											= m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc);

			CreatePipeline();

			Nexus::Graphics::SamplerDescription samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

			Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
			cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer;
			cameraUniformBufferView.Offset							   = 0;
			cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
			m_ResourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

			Nexus::Graphics::UniformBufferView transformUniformBufferView = {};
			transformUniformBufferView.BufferHandle						  = m_TransformUniformBuffer;
			transformUniformBufferView.Offset							  = 0;
			transformUniformBufferView.Size								  = m_TransformUniformBuffer->GetDescription().SizeInBytes;
			m_ResourceSet->WriteUniformBuffer(transformUniformBufferView, "Transform");

			Nexus::Graphics::CombinedImageSampler ciSampler = {};
			ciSampler.ImageTexture							= m_TextureView;
			ciSampler.ImageSampler							= m_Sampler;
			m_ResourceSet->WriteCombinedImageSampler(ciSampler, "u_Texture");

			m_ResourceSet->Flush();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_TransformUniforms.Transform =
				glm::rotate(glm::mat4(1.0f), glm::radians((float)m_ElapsedTime.GetSeconds<float>() * 100.0f), glm::vec3(0.0f, 1.0f, 1.0f));
			m_TransformUniformBuffer->SetData(&m_TransformUniforms, 0, sizeof(m_TransformUniforms));

			auto [width, height] = m_Window->GetWindowSize();

			m_CameraUniforms.View		= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f));
			m_CameraUniforms.Projection = glm::perspectiveFov<float>(glm::radians(60.0f), width, height, 0.1f, 100.0f);
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);

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

			Nexus::Graphics::ClearDepthStencilValue clearValue;
			m_CommandList->ClearDepthTarget(clearValue);

			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = m_ResourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_Mesh->GetVertexBuffer();
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Size							   = m_Mesh->GetVertexBuffer()->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_Mesh->GetIndexBuffer();
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_Mesh->GetIndexBuffer()->GetSizeInBytes();
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			auto indexCount = m_Mesh->GetIndexBuffer()->GetCount();

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

			m_ElapsedTime += time;
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

			pipelineDescription.VertexModule   = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																									"resources/demo/shaders/3d/3d.vert.glsl",
																									Nexus::GetApplication()->GetApplicationPath(),
																									Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																									"resources/demo/shaders/3d/3d.frag.glsl",
																									Nexus::GetApplication()->GetApplicationPath(),
																									Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ColourTargetCount = 1;
			pipelineDescription.ColourFormats[0]  = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.Samples			  = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			pipelineDescription.ResourceDescription.Descriptors = {
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_Texture",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "Camera",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "Transform",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
													 .CountOrSizeInBytes = 1}};

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a rotating textured cube using matrices";
		}

	  private:
		Nexus::Graphics::CommandListHandle m_CommandList = {};
		Nexus::Graphics::PipelineHandle	   m_Pipeline	 = {};
		Nexus::Ref<Nexus::Graphics::Mesh>  m_Mesh		 = nullptr;
		Nexus::Graphics::TextureHandle	   m_Texture	 = {};
		Nexus::Graphics::TextureViewHandle m_TextureView = {};
		Nexus::Graphics::SamplerHandle	   m_Sampler	 = {};
		glm::vec3						   m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Graphics::ResourceSetHandle	m_ResourceSet		  = {};
		VB_UNIFORM_CAMERA_DEMO_3D			m_CameraUniforms	  = {};
		Nexus::Graphics::DeviceBufferHandle m_CameraUniformBuffer = {};

		VB_UNIFORM_TRANSFORM_DEMO_3D		m_TransformUniforms		 = {};
		Nexus::Graphics::DeviceBufferHandle m_TransformUniformBuffer = {};

		Nexus::TimeSpan m_ElapsedTime = {};
	};
}	 // namespace Demos