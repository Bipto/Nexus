#include "Nexus-Core/Renderer/SceneRenderer.hpp"

#include "Nexus-Core/ECS/ComponentRegistry.hpp"
#include "Nexus-Core/ECS/Components.hpp"
#include "Nexus-Core/ECS/Registry.hpp"

namespace Nexus::Graphics
{
	SceneRenderer::SceneRenderer(Graphics::IGraphicsDevice *device, Graphics::CommandQueueHandle commandQueue)
		: m_Device(device),
		  m_CommandQueue(commandQueue)
	{
		m_Renderer3D	= std::make_unique<Nexus::Graphics::Renderer3D>(device, commandQueue);
		m_BatchRenderer = std::make_unique<Nexus::Graphics::BatchRenderer>(device, commandQueue, true, 1);
	}

	void SceneRenderer::Render(Scene *scene, FramebufferHandle target, TimeSpan time)
	{
		m_Renderer3D->Begin(scene, target, time);
		m_Renderer3D->End();

		auto [width, height] = target->GetSize();

		Nexus::Graphics::Viewport vp = {};
		vp.X						 = 0;
		vp.Y						 = 0;
		vp.Width					 = width;
		vp.Height					 = height;
		vp.MinDepth					 = 0.0f;
		vp.MaxDepth					 = 1.0f;

		Nexus::Graphics::Scissor scissor = {};
		scissor.X						 = 0;
		scissor.Y						 = 0;
		scissor.Width					 = width;
		scissor.Height					 = height;

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

				/*m_BatchRenderer->DrawQuadFill(spriteRenderer->SpriteColour,
											  spriteRenderer->SpriteTexture,
											  spriteRenderer->Tiling,
											  worldMatrix,
											  entity->ID);*/
			});

		m_BatchRenderer->End();
	}

	const Nexus::FirstPersonCamera SceneRenderer::GetCamera() const
	{
		return m_Renderer3D->GetCamera();
	}
}	 // namespace Nexus::Graphics
