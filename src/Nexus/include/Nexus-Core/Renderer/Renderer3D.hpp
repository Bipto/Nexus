#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime/Camera.hpp"
#include "Nexus-Core/Runtime/Scene.hpp"

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
		Renderer3D(GraphicsDevice *device);
		~Renderer3D();

		void Begin(Scene *scene, RenderTarget target, Nexus::TimeSpan time);
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
		GraphicsDevice *m_Device		 = nullptr;
		RenderTarget	m_RenderTarget	 = {};
		FullscreenQuad	m_FullscreenQuad = {};

		Scene		*m_Scene   = nullptr;
		Ref<Texture> m_Cubemap = nullptr;

		Ref<CommandList> m_CommandList = nullptr;

		Ref<Nexus::Graphics::Mesh> m_Cube;

		Nexus::FirstPersonCamera m_Camera;

		Nexus::Ref<Nexus::Graphics::Sampler>	   m_CubemapSampler		 = nullptr;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_CubemapPipeline		 = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_CubemapUniformBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::ResourceSet>   m_CubemapResourceSet	 = nullptr;

		Nexus::Ref<Nexus::Graphics::Sampler>	   m_ModelSampler				 = nullptr;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_ModelPipeline				= nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_ModelCameraUniformBuffer	 = nullptr;
		std::map<Nexus::Ref<Nexus::Graphics::Model>, Nexus::Ref<Nexus::Graphics::DeviceBuffer>> m_ModelTransformUniformBuffers = {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, Nexus::Ref<Nexus::Graphics::ResourceSet>>	m_ModelResourceSets			   = {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, ModelRenderData>							m_ModelIDs					   = {};

		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_ClearScreenPipeline = nullptr;

		Nexus::Ref<Nexus::Graphics::Texture> m_DefaultTexture = nullptr;
	};
}	 // namespace Nexus::Graphics