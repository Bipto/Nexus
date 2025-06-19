#pragma once

#include "Demo.hpp"

namespace Demos
{
	class StorageBufferDemo : public Demo
	{
	  public:
		StorageBufferDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~StorageBufferDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			CreatePipeline();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_Mesh = factory.CreateSprite();

			m_Texture = m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/brick.jpg"), false);

			Nexus::Graphics::SamplerSpecification samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
			m_UploadBuffer->SetData(&transform, 0, sizeof(transform));

			m_CommandList->Begin();

			Nexus::Graphics::BufferCopyDescription bufferCopy = {};
			bufferCopy.Source								  = m_UploadBuffer;
			bufferCopy.Destination							  = m_StorageBuffer;
			bufferCopy.ReadOffset							  = 0;
			bufferCopy.WriteOffset							  = 0;
			bufferCopy.Size									  = sizeof(glm::mat4);
			m_CommandList->CopyBufferToBuffer(bufferCopy);

			m_CommandList->SetPipeline(m_Pipeline);
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

			Nexus::Graphics::StorageBufferView storageBufferView = {};
			storageBufferView.BufferHandle						 = m_StorageBuffer;
			storageBufferView.Offset							 = 0;
			storageBufferView.SizeInBytes						 = sizeof(glm::mat4);
			storageBufferView.Access							 = Nexus::Graphics::ShaderAccess::Read;
			m_ResourceSet->WriteStorageBuffer(storageBufferView, "TransformBuffer");

			m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");

			m_CommandList->SetResourceSet(m_ResourceSet);

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_Mesh->GetVertexBuffer();
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Size							   = m_Mesh->GetVertexBuffer()->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_Mesh->GetIndexBuffer();
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_Mesh->GetIndexBuffer()->GetSizeInBytes();
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
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

			m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void RenderUI() override
		{
			ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.1f, -1.0f, 1.0f);
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.VertexModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/storage_buffers.vert.glsl",
																	   Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/shader_buffers.frag.glsl",
																	   Nexus::Graphics::ShaderStage::Fragment);

			Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
			transformUniformBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Default;
			transformUniformBufferDesc.Usage									= Nexus::Graphics::BufferUsage::Storage;
			transformUniformBufferDesc.StrideInBytes							= sizeof(glm::mat4);
			transformUniformBufferDesc.SizeInBytes								= sizeof(glm::mat4);
			m_StorageBuffer														= m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc);

			Nexus::Graphics::DeviceBufferDescription uploadBufferDesc = {};
			uploadBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
			uploadBufferDesc.Usage									  = BUFFER_USAGE_NONE;
			uploadBufferDesc.StrideInBytes							  = sizeof(glm::mat4);
			uploadBufferDesc.SizeInBytes							  = sizeof(glm::mat4);
			m_UploadBuffer											  = m_GraphicsDevice->CreateDeviceBuffer(uploadBufferDesc);

			pipelineDescription.ResourceSetSpec.StorageBuffers = {{"TransformBuffer", 0, 0}};

			pipelineDescription.ResourceSetSpec.SampledImages = {{"texSampler", 1, 0}};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a textured quad and uploading matrix transforms to the GPU to change the position.";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList>	  m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::Texture>		  m_Texture;
		Nexus::Ref<Nexus::Graphics::ResourceSet>	  m_ResourceSet;
		Nexus::Ref<Nexus::Graphics::Mesh>			  m_Mesh;
		Nexus::Ref<Nexus::Graphics::Sampler>		  m_Sampler;
		glm::vec3									  m_ClearColour = {0.7f, 0.2f, 0.3f};

		glm::vec3 m_Position {0.0f, 0.0f, 0.0f};

		glm::mat4								  m_TransformUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_UploadBuffer;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_StorageBuffer;
	};
}	 // namespace Demos