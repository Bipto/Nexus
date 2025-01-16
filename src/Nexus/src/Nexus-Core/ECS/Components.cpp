#include "Components.hpp"

void RegisterDefaultComponents()
{
	REGISTER_COMPONENT_WITH_RENDER_FUNC(Nexus::Transform,
										"Transform",
										[](void *data)
										{
											Nexus::Transform *transform = static_cast<Nexus::Transform *>(data);
											ImGui::DragFloat3("Position", glm::value_ptr(transform->Position));
											ImGui::DragFloat3("Rotation", glm::value_ptr(transform->Rotation));
											ImGui::DragFloat3("Scale", glm::value_ptr(transform->Scale));
										});

	REGISTER_COMPONENT_WITH_RENDER_FUNC(Nexus::ModelRenderer,
										"ModelRenderer",
										[](void *data)
										{
											Nexus::ModelRenderer *modelRenderer = static_cast<Nexus::ModelRenderer *>(data);
											ImGui::Text(modelRenderer->FilePath.c_str());
											ImGui::SameLine();
											ImGui::Button("+");
										});
}