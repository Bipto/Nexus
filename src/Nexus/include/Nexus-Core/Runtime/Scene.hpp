#pragma once

#include "Entity.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "glm/glm.hpp"

#include "Nexus-Core/ECS/Registry.hpp"

#include "Nexus-Core/Utils/GUID.hpp"

#include "Nexus-Core/Graphics/Sampler.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"

namespace Nexus
{
	struct SceneInfo
	{
		GUID		Guid = {};
		std::string Name = {};
		std::string Path = {};
	};

	enum class SceneState
	{
		Playing,
		Paused,
		Stopped
	};

	struct Environment
	{
		std::string			   CubemapPath		  = {};
		Ref<Graphics::Cubemap> EnvironmentCubemap = nullptr;
		Ref<Graphics::Sampler> CubemapSampler	  = nullptr;
		glm::vec4			   ClearColour		  = {1.0f, 1.0f, 1.0f, 1.0f};
	};

	// forward declaration
	class Project;

	struct NX_API Scene
	{
	  public:
		Scene();
		void Serialize(const std::string &filepath);

		void				 AddEmptyEntity();
		Entity				*GetEntity(GUID id);
		std::vector<Entity> &GetEntities();

		void Start();
		void Stop();
		void Pause();
		void OnUpdate(TimeSpan time);
		void OnRender(TimeSpan time);
		void OnTick(TimeSpan time);

		SceneState GetSceneState();

	  public:
		static Scene *Deserialize(const SceneInfo &info, const std::string &sceneDirectory, Project *project);

	  public:
		GUID		  Guid			   = {};
		std::string	  Name			   = {};
		Environment	  SceneEnvironment = {};
		ECS::Registry Registry		   = {};
		Project		 *ParentProject	   = nullptr;

	  private:
		SceneState m_SceneState = SceneState::Stopped;
	};
}	 // namespace Nexus