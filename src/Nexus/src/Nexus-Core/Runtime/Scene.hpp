#pragma once

#include "Entity.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "glm/glm.hpp"

#include "Nexus-Core/ECS/Registry.hpp"

#include "Nexus-Core/Utils/GUID.hpp"

#include "Nexus-Core/Graphics/Sampler.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus
{
	struct Environment
	{
		std::string			   CubemapPath		  = {};
		Ref<Graphics::Cubemap> EnvironmentCubemap = nullptr;
		Ref<Graphics::Sampler> CubemapSampler	  = nullptr;
		glm::vec4			   ClearColour		  = {1.0f, 1.0f, 1.0f, 1.0f};
	};

	struct Scene
	{
	  public:
		Scene();
		void Serialize(const std::string &filepath);

		void				 AddEmptyEntity();
		Entity				*GetEntity(GUID id);
		std::vector<Entity> &GetEntities();

	  public:
		static Scene *Deserialize(GUID guid, const std::string &sceneDirectory);

	  public:
		GUID				Guid			 = {};
		std::string			Name			 = {};
		Environment			SceneEnvironment = {};
		ECS::Registry		Registry		 = {};
	};
}	 // namespace Nexus