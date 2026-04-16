#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime/Camera.hpp"
#include "Nexus-Core/Runtime/Scene.hpp"
#include "RHI/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	struct alignas(16) CubemapCameraUniforms
	{
		glm::mat4 View		 = {};
		glm::mat4 Projection = {};
	};

	struct alignas(16) ModelCameraUniforms
	{
		glm::mat4 View		  = {};
		glm::mat4 Projection  = {};
		glm::vec3 CamPosition = {};
	};

	struct alignas(16) ModelTransformUniforms
	{
		glm::mat4 Transform		 = {};
		glm::vec4 DiffuseColour	 = {};
		glm::vec4 SpecularColour = {};
		uint32_t  Guid1			 = {};
		uint32_t  Guid2			 = {};
	};

	struct ModelRenderData
	{
		glm::mat4 Transform = {};
		GUID	  Guid		= {};
	};

	class NX_API Renderer3D
	{
	  public:
		Renderer3D(IGraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		~Renderer3D();

		void Begin(Scene *scene, FramebufferHandle target, Nexus::TimeSpan time);
		void End();

		const Nexus::FirstPersonCamera GetCamera() const;

	  private:
		void RenderModel(Nexus::Ref<Nexus::Graphics::Model> model, const glm::mat4 transform, GUID guid);
		void RenderCubemap();
		void ClearGBuffer();

		void CreateCubemapPipeline();
		void CreateModelPipeline();
		void CreateClearGBufferPipeline();

	  private:
		IGraphicsDevice				*m_Device		  = nullptr;
		Ref<Graphics::ICommandQueue> m_CommandQueue	  = nullptr;
		FramebufferHandle			 m_RenderTarget	  = {};
		FullscreenQuad				 m_FullscreenQuad = {};

		Scene					   *m_Scene	  = nullptr;
		Graphics::TextureViewHandle m_Cubemap = {};

		Ref<ICommandList> m_CommandList = nullptr;

		Ref<Nexus::Graphics::Mesh> m_Cube = nullptr;

		Nexus::FirstPersonCamera m_Camera = {};

		Graphics::SamplerHandle					   m_CubemapSampler		  = {};
		PipelineHandle							   m_CubemapPipeline	  = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_CubemapUniformBuffer = nullptr;
		Nexus::Graphics::ResourceSetHandle		   m_CubemapResourceSet	  = {};

		Graphics::SamplerHandle																	 m_ModelSampler					= {};
		Graphics::PipelineHandle																 m_ModelPipeline				= {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>												 m_ModelCameraUniformBuffer		= nullptr;
		std::map<Nexus::Ref<Nexus::Graphics::Model>, Nexus::Ref<Nexus::Graphics::IDeviceBuffer>> m_ModelTransformUniformBuffers = {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, ResourceSetHandle>							 m_ModelResourceSets			= {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, ModelRenderData>							 m_ModelIDs						= {};

		PipelineHandle m_ClearScreenPipeline = {};

		Graphics::TextureHandle		m_DefaultTexture	 = {};
		Graphics::TextureViewHandle m_DefaultTextureView = {};
	};
}	 // namespace Nexus::Graphics