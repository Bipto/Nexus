#pragma once

#include "Core/Graphics/GraphicsDevice.hpp"

#include <string>

namespace Nexus
{
    /// @brief A class representing an asset manager to load and retrieve assets
    class AssetManager
    {
    public:
        /// @brief An AssetManager cannot be created without a graphics device
        AssetManager() = delete;

        /// @brief A constructor taking in a reference counted pointer to a graphics device
        /// @param graphicsDevice A pointer to a graphics device
        AssetManager(Ref<Graphics::GraphicsDevice> graphicsDevice) : m_GraphicsDevice(graphicsDevice) {}

        /// @brief A method to return a reference counted pointer to a texture
        /// @param filepath A filepath to retrieve a texture from, if the texture has already been loaded then the cached one will be returned
        /// @return A reference counted pointer to a texture
        Ref<Graphics::Texture> GetTexture(const std::string &filepath);

    private:
        /// @brief A reference counted pointer to a graphics device
        Ref<Graphics::GraphicsDevice> m_GraphicsDevice;
    };
}