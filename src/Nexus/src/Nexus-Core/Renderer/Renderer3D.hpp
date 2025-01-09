#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Runtime/Camera.hpp"

namespace Nexus::Graphics
{
	struct Scene
	{
		Ref<Cubemap> Environment		= nullptr;
		Ref<Sampler> EnvironmentSampler = nullptr;
		glm::vec4	 EnvironmentColour	= {1.0f, 1.0f, 1.0f, 1.0f};
	};

	struct CameraUniforms
	{
		glm::mat4 View		 = {};
		glm::mat4 Projection = {};
	};

	class Renderer3D
	{
	  public:
		Renderer3D(GraphicsDevice *device);
		~Renderer3D();

		void Begin(const Scene &scene, RenderTarget target, Ref<Cubemap> cubemap, Nexus::TimeSpan time);
		void End();

	  private:
		void CreateCubemapPipeline();

	  private:
		GraphicsDevice *m_Device	   = nullptr;
		RenderTarget	m_RenderTarget = {};
		Scene			m_Scene		   = {};
		Ref<Cubemap>	m_Cubemap	   = nullptr;

		Ref<CommandList> m_CommandList = nullptr;

		Ref<Nexus::Graphics::Mesh> m_Cube;

		Nexus::FirstPersonCamera m_Camera;

		Nexus::Ref<Nexus::Graphics::Sampler>	 m_Sampler			  = nullptr;
		Nexus::Ref<Nexus::Graphics::Pipeline>	 m_CubemapPipeline	  = nullptr;
		Nexus::Ref<Nexus::Graphics::ResourceSet> m_CubemapResourceSet = nullptr;

		Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer = nullptr;
	};
}	 // namespace Nexus::Graphics