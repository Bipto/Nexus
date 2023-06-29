#pragma once

#include "Core/Memory.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Mesh.h"

namespace Nexus
{
    /// @brief A class allowing a specific mesh to be generated or a model to be loaded from a file
    class MeshFactory
    {
    public:
        /// @brief
        /// @param device
        explicit MeshFactory(Ref<GraphicsDevice> device)
        {
            m_Device = device;
        }

        /// @brief A method that returns a mesh representing a cube with a size of 1 unit in all directions
        /// @return A mesh representing a cube
        Mesh CreateCube();

        /// @brief A method that returns a mesh representing a sprite with a width and height of 1
        /// @return A mesh representing a sprite
        Mesh CreateSprite();

        /// @brief A method that returns a mesh representing a 3D model stored on disk
        /// @param filepath The filepath to load a model from
        /// @return A mesh representing the model
        Mesh CreateFromFile(const std::string &filepath);

    private:
        /// @brief A reference counted pointer to a graphics device to use to create the vertex buffer and index buffer
        Ref<GraphicsDevice> m_Device = nullptr;
    };
}