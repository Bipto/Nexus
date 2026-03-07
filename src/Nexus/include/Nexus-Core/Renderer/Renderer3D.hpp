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

		void Begin(Scene *scene, Ref<IFramebuffer> target, Nexus::TimeSpan time);
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
		Ref<IFramebuffer>			 m_RenderTarget	  = {};
		FullscreenQuad				 m_FullscreenQuad = {};

		Scene			 *m_Scene	= nullptr;
		Ref<ITextureView> m_Cubemap = nullptr;

		Ref<ICommandList> m_CommandList = nullptr;

		Ref<Nexus::Graphics::Mesh> m_Cube = nullptr;

		Nexus::FirstPersonCamera m_Camera = {};

		Nexus::Ref<Nexus::Graphics::ISampler>		   m_CubemapSampler		  = nullptr;
		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline> m_CubemapPipeline	  = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>	   m_CubemapUniformBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IResourceSet>	   m_CubemapResourceSet	  = nullptr;

		Nexus::Ref<Nexus::Graphics::ISampler>													 m_ModelSampler					= nullptr;
		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline>											 m_ModelPipeline				= nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>												 m_ModelCameraUniformBuffer		= nullptr;
		std::map<Nexus::Ref<Nexus::Graphics::Model>, Nexus::Ref<Nexus::Graphics::IDeviceBuffer>> m_ModelTransformUniformBuffers = {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, Nexus::Ref<Nexus::Graphics::IResourceSet>>	 m_ModelResourceSets			= {};
		std::map<Nexus::Ref<Nexus::Graphics::Model>, ModelRenderData>							 m_ModelIDs						= {};

		Nexus::Ref<Nexus::Graphics::IGraphicsPipeline> m_ClearScreenPipeline = nullptr;

		Nexus::Ref<Nexus::Graphics::ITexture>	  m_DefaultTexture	   = nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView> m_DefaultTextureView = nullptr;
	};
}	 // namespace Nexus::Graphics