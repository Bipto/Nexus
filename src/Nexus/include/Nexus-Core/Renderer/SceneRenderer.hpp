#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Renderer/Renderer3D.hpp"
#include "Nexus-Core/Runtime/Scene.hpp"

namespace Nexus::Graphics
{
	class NX_API SceneRenderer
	{
	  public:
		SceneRenderer(GraphicsDevice *device);
		~SceneRenderer() = default;
		void						   Render(Scene *scene, RenderTarget &target, TimeSpan time);
		const Nexus::FirstPersonCamera GetCamera() const;

	  private:
		Graphics::GraphicsDevice					   *m_Device		= nullptr;
		std::unique_ptr<Nexus::Graphics::Renderer3D>	m_Renderer3D	= nullptr;
		std::unique_ptr<Nexus::Graphics::BatchRenderer> m_BatchRenderer = nullptr;
	};
}	 // namespace Nexus::Graphics