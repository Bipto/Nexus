#include "Renderer3D.hpp"

namespace Nexus::Graphics
{
	Renderer3D::Renderer3D(GraphicsDevice *device) : m_Device(device), m_Camera(m_Device)
	{
		m_CommandList = m_Device->CreateCommandList();
	}

	Renderer3D::~Renderer3D()
	{
	}

	void Nexus::Graphics::Renderer3D::Begin(const Scene &scene, RenderTarget target)
	{
		m_Scene		   = scene;
		m_RenderTarget = target;
	}

	void Nexus::Graphics::Renderer3D::End()
	{
		m_CommandList->Begin();
		m_CommandList->SetRenderTarget(m_RenderTarget);
		m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
		m_CommandList->End();
		m_Device->SubmitCommandList(m_CommandList);
	}

}	 // namespace Nexus::Graphics