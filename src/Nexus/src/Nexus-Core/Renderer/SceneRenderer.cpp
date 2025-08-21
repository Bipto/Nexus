#include "Nexus-Core/Renderer/SceneRenderer.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/ECS/Components.hpp"
#include "Nexus-Core/ECS/Registry.hpp"

namespace Nexus::Graphics
{
	SceneRenderer::SceneRenderer(Graphics::GraphicsDevice *device) : m_Device(device)
	{
		m_Renderer3D	= std::make_unique<Nexus::Graphics::Renderer3D>(device);
		m_BatchRenderer = std::make_unique<Nexus::Graphics::BatchRenderer>(device, true, 1);
	}

	void SceneRenderer::Render(Scene *scene, RenderTarget &target, TimeSpan time)
	{
		m_Renderer3D->Begin(scene, target, time);
		m_Renderer3D->End();

		Point2D<uint32_t> targetSize = target.GetSize();

		Nexus::Graphics::Viewport vp = {};
		vp.X						 = 0;
		vp.Y						 = 0;
		vp.Width					 = targetSize.X;
		vp.Height					 = targetSize.Y;
		vp.MinDepth					 = 0.0f;
		vp.MaxDepth					 = 1.0f;

		Nexus::Graphics::Scissor scissor = {};
		scissor.X						 = 0;
		scissor.Y						 = 0;
		scissor.Width					 = targetSize.X;
		scissor.Height					 = targetSize.Y;

		glm::mat4 viewProj = m_Renderer3D->GetCamera().GetViewProjection();

		m_BatchRenderer->Begin(target, vp, scissor, viewProj);

		Nexus::ECS::View<Nexus::Transform, Nexus::SpriteRendererComponent> transformsSpriteRenderers =
			scene->Registry.GetView<Nexus::Transform, Nexus::SpriteRendererComponent>();

		transformsSpriteRenderers.Each(
			[&](Nexus::Entity *entity, const std::tuple<Nexus::Transform *, Nexus::SpriteRendererComponent *> &components)
			{
				Nexus::Transform			   *transform	   = std::get<0>(components);
				Nexus::SpriteRendererComponent *spriteRenderer = std::get<1>(components);

				const Nexus::FirstPersonCamera &camera		= m_Renderer3D->GetCamera();
				glm::mat4						worldMatrix = transform->CreateTransformation();

				m_BatchRenderer->DrawQuadFill(spriteRenderer->SpriteColour,
											  spriteRenderer->SpriteTexture,
											  spriteRenderer->Tiling,
											  worldMatrix,
											  entity->ID);
			});

		m_BatchRenderer->End();
	}

	const Nexus::FirstPersonCamera SceneRenderer::GetCamera() const
	{
		return m_Renderer3D->GetCamera();
	}
}	 // namespace Nexus::Graphics
