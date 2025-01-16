#pragma once

#include "ComponentRegistry.hpp"
#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Registry.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Model.hpp"

namespace Nexus
{
	struct Transform
	{
		glm::vec3 Position = {};
		glm::vec3 Rotation = {};
		glm::vec3 Scale	   = {};

		friend std::ostream &operator<<(std::ostream &os, const Transform &transform)
		{
			os << transform.Position.x << " " << transform.Position.y << " " << transform.Position.z << " ";
			os << transform.Rotation.x << " " << transform.Rotation.y << " " << transform.Rotation.z << " ";
			os << transform.Scale.x << " " << transform.Scale.y << " " << transform.Scale.z;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, Transform &transform)
		{
			is >> transform.Position.x >> transform.Position.y >> transform.Position.z;
			is >> transform.Rotation.x >> transform.Rotation.y >> transform.Rotation.z;
			is >> transform.Scale.x >> transform.Scale.y >> transform.Scale.z;

			return is;
		}
	};

	struct ModelRenderer
	{
		std::string						   FilePath = {};
		Nexus::Ref<Nexus::Graphics::Model> Model	= {};

		friend std::ostream &operator<<(std::ostream &os, const ModelRenderer &modelRenderer)
		{
			os << modelRenderer.FilePath;
			return os;
		}

		friend std::istream &operator>>(std::istream &is, ModelRenderer &modelRenderer)
		{
			is >> modelRenderer.FilePath;
			return is;
		}
	};

}	 // namespace Nexus

void RegisterDefaultComponents();