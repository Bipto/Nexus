#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Renderer/Renderer3D.hpp"
#include "Nexus-Core/Runtime/Scene.hpp"
#include "RHI/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class NX_API SceneRenderer
	{
	  public:
		SceneRenderer(IGraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		~SceneRenderer() = default;
		void						   Render(Scene *scene, Ref<IFramebuffer> target, TimeSpan time);
		const Nexus::FirstPersonCamera GetCamera() const;

	  private:
		Graphics::IGraphicsDevice					   *m_Device		= nullptr;
		Ref<Graphics::ICommandQueue>					m_CommandQueue	= nullptr;
		std::unique_ptr<Nexus::Graphics::Renderer3D>	m_Renderer3D	= nullptr;
		std::unique_ptr<Nexus::Graphics::BatchRenderer> m_BatchRenderer = nullptr;
	};
}	 // namespace Nexus::Graphics