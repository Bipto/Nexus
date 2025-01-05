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

	class Renderer3D
	{
	  public:
		Renderer3D(GraphicsDevice *device);
		~Renderer3D();

		void Begin(const Scene &scene, RenderTarget target);
		void End();

	  private:
		GraphicsDevice *m_Device	   = nullptr;
		RenderTarget	m_RenderTarget = {};
		Scene			m_Scene		   = {};

		Ref<CommandList> m_CommandList = nullptr;
		Ref<Pipeline>	 m_Pipeline	   = nullptr;
		Ref<ResourceSet> m_ResourceSet = nullptr;

		Ref<Nexus::Graphics::Mesh> m_Cube;

		Nexus::FirstPersonCamera m_Camera;
	};
}	 // namespace Nexus::Graphics