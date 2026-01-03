#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_INSTANCING
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 CamPosition;
	};

	class InstancingDemo : public Demo
	{
	  public:
		InstancingDemo(const std::string						 &name,
					   Nexus::Application						 *app,
					   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
					   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~InstancingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
			m_CubeMesh = factory.CreateCube();

			auto [diffuseMap, diffuseMapView] = m_GraphicsDevice->CreateTexture2DWithView(
				m_CommandQueue,
				Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg"),
				true);
			m_DiffuseMap	 = diffuseMap;
			m_DiffuseMapView = diffuseMapView;

			auto [normalMap, normalMapView] = m_GraphicsDevice->CreateTexture2DWithView(
				m_CommandQueue,
				Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_normal_1k.jpg"),
				true);
			m_NormalMap		= normalMap;
			m_NormalMapView = normalMapView;

			auto [specularMap, specularMapView] = m_GraphicsDevice->CreateTexture2DWithView(
				m_CommandQueue,
				Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_spec_1k.jpg"),
				true);
			m_SpecularMap	  = specularMap;
			m_SpecularMapView = specularMapView;

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			cameraUniformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING);
			m_CameraUniformBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc);

			Nexus::Graphics::DeviceBufferDescription instanceBufferDesc = {};
			instanceBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			instanceBufferDesc.Usage									= Nexus::Graphics::BufferUsage_Vertex;
			instanceBufferDesc.StrideInBytes							= sizeof(glm::mat4);
			instanceBufferDesc.SizeInBytes								= m_InstanceCount * sizeof(glm::mat4);
			m_InstanceBuffer											= m_GraphicsDevice->CreateDeviceBuffer(instanceBufferDesc);

			std::vector<glm::mat4> mvps(m_InstanceCount);
			for (uint32_t i = 0; i < m_InstanceCount; i++) { mvps[i] = glm::translate(glm::mat4(1.0f), glm::vec3(i * 2.0f, 0.0f, -2.5f)); }
			m_InstanceBuffer->SetData(mvps.data(), 0, mvps.size() * sizeof(glm::mat4));

			CreatePipeline();
			m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));

			Nexus::Graphics::SamplerDescription samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

			Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
			cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer;
			cameraUniformBufferView.Offset							   = 0;
			cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
			m_ResourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

			Nexus::Graphics::CombinedImageSampler diffuseCiSampler = {};
			diffuseCiSampler.ImageTexture						   = m_DiffuseMapView;
			diffuseCiSampler.ImageSampler						   = m_Sampler;
			m_ResourceSet->WriteCombinedImageSampler(diffuseCiSampler, "u_DiffuseMap");

			Nexus::Graphics::CombinedImageSampler normalCiSampler = {};
			normalCiSampler.ImageTexture						  = m_NormalMapView;
			normalCiSampler.ImageSampler						  = m_Sampler;
			m_ResourceSet->WriteCombinedImageSampler(normalCiSampler, "u_NormalMap");

			Nexus::Graphics::CombinedImageSampler specularCiSampler = {};
			specularCiSampler.ImageTexture							= m_SpecularMapView;
			specularCiSampler.ImageSampler							= m_Sampler;
			m_ResourceSet->WriteCombinedImageSampler(specularCiSampler, "u_SpecularMap");

			m_ResourceSet->Flush();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			auto [width, height] = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();

			m_Rotation += time.GetSeconds<float>();
			m_CameraUniforms.View		 = m_Camera.GetView();
			m_CameraUniforms.Projection	 = m_Camera.GetProjection();
			m_CameraUniforms.CamPosition = m_Camera.GetPosition();
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);

			Nexus::Ref<Nexus::Graphics::ISwapchain>	  swapchain	  = Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Ref<Nexus::Graphics::IFramebuffer> framebuffer = swapchain->GetCurrentFramebuffer();
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

			// draw cube
			{
				Nexus::Graphics::VertexBufferView vertexBufferView = {};
				vertexBufferView.BufferHandle					   = m_CubeMesh->GetVertexBuffer();
				vertexBufferView.Offset							   = 0;
				vertexBufferView.Size							   = m_CubeMesh->GetVertexBuffer()->GetSizeInBytes();
				m_CommandList->SetVertexBuffer(vertexBufferView, 0);

				Nexus::Graphics::VertexBufferView instanceBufferView = {};
				instanceBufferView.BufferHandle						 = m_InstanceBuffer;
				instanceBufferView.Offset							 = 0;
				instanceBufferView.Size								 = m_InstanceBuffer->GetSizeInBytes();
				m_CommandList->SetVertexBuffer(instanceBufferView, 1);

				Nexus::Graphics::IndexBufferView indexBufferView = {};
				indexBufferView.BufferHandle					 = m_CubeMesh->GetIndexBuffer();
				indexBufferView.Offset							 = 0;
				indexBufferView.Size							 = m_CubeMesh->GetIndexBuffer()->GetSizeInBytes();
				indexBufferView.BufferFormat					 = Nexus::Graphics::IndexFormat::UInt32;
				m_CommandList->SetIndexBuffer(indexBufferView);

				auto indexCount	   = m_CubeMesh->GetIndexBuffer()->GetCount();
				auto instanceCount = m_InstanceBuffer->GetSizeInBytes() / m_InstanceBuffer->GetStrideInBytes();

				Nexus::Graphics::DrawIndexedDescription drawDesc = {};
				drawDesc.VertexStart							 = 0;
				drawDesc.IndexStart								 = 0;
				drawDesc.InstanceStart							 = 0;
				drawDesc.IndexCount								 = indexCount;
				drawDesc.InstanceCount							 = instanceCount;
				m_CommandList->DrawIndexed(drawDesc);
			}

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();

			m_Rotation += 0.05f * time.GetMilliseconds<float>();
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
			auto [width, height] = m_Window->GetWindowSize();
			m_Camera.Update(width, height, time);
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a series of textured cubes using a vertex buffer containing vertices, an index buffer\nand a second vertex buffer "
				   "containing matrix transformations per cube.";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

			pipelineDescription.VertexModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/instancing/instancing.vert.glsl",
																	   Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/instancing/instancing.frag.glsl",
																	   Nexus::Graphics::ShaderStage::Fragment);

			Nexus::Graphics::VertexBufferLayout instanceLayout = {{{Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"}},
																  sizeof(glm::mat4),
																  Nexus::Graphics::StepRate::Instance};

			pipelineDescription.Layouts = {{Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()}, instanceLayout};

			pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
			pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
			pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

			pipelineDescription.ColourTargetCount = 1;
			pipelineDescription.ColourFormats[0]  = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.Samples			  = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			pipelineDescription.ResourceDescription.Descriptors = {
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_DiffuseMap",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_NormalMap",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "u_SpecularMap",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
													 .CountOrSizeInBytes = 1},
				Nexus::Graphics::ResourceDescriptor {.Name				 = "Camera",
													 .Type				 = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
													 .CountOrSizeInBytes = 1}};

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::ICommandList>	   m_CommandList	= nullptr;
		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline> m_Pipeline		= nullptr;
		Nexus::Ref<Nexus::Graphics::Mesh>			   m_CubeMesh		= nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>	   m_InstanceBuffer = nullptr;

		Nexus::Ref<Nexus::Graphics::IResourceSet> m_ResourceSet		= nullptr;
		Nexus::Ref<Nexus::Graphics::ITexture>	  m_DiffuseMap		= nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView> m_DiffuseMapView	= nullptr;
		Nexus::Ref<Nexus::Graphics::ITexture>	  m_NormalMap		= nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView> m_NormalMapView	= nullptr;
		Nexus::Ref<Nexus::Graphics::ITexture>	  m_SpecularMap		= nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView> m_SpecularMapView = nullptr;
		glm::vec3								  m_ClearColour		= {0.7f, 0.2f, 0.3f};

		VB_UNIFORM_CAMERA_DEMO_INSTANCING		   m_CameraUniforms		 = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_CameraUniformBuffer = nullptr;

		Nexus::Ref<Nexus::Graphics::ISampler> m_Sampler = nullptr;

		Nexus::FirstPersonCamera m_Camera = {};

		const uint32_t m_InstanceCount = 10;

		float m_Rotation = 0.0f;
	};
}	 // namespace Demos