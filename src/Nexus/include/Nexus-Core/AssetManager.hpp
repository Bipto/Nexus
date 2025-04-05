#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{

	class Project;

	/// @brief A class representing an asset manager to load and retrieve assets
	class AssetManager
	{
	  public:
		/// @brief An AssetManager cannot be created without a graphics device and project
		AssetManager() = delete;

		/// @brief A constructor taking in a reference counted pointer to a graphics
		/// device
		/// @param graphicsDevice A pointer to a graphics device
		AssetManager(Graphics::GraphicsDevice *graphicsDevice, Project *project) : m_GraphicsDevice(graphicsDevice), m_Project(project)
		{
		}

		template<typename T>
		Ref<T> GetAsset(GUID id)
		{
			std::any value = LoadAsset(id);
			if (value.type() == typeid(Ref<T>))
			{
				return std::any_cast<Ref<T>>(value);
			}

			return {};
		}

		/// @brief A method to return a reference counted pointer to a texture
		/// @param filepath A filepath to retrieve a texture from, if the texture has
		/// already been loaded then the cached one will be returned
		/// @return A reference counted pointer to a texture
		Ref<Graphics::Texture2D> GetTexture(const std::string &filepath);

	  private:
		std::any LoadAsset(GUID id);

	  private:
		/// @brief A reference counted pointer to a graphics device
		Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;

		Project *m_Project = nullptr;
	};
}	 // namespace Nexus