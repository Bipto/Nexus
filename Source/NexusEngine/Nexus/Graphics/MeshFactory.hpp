#pragma once

#include "Nexus/Types.hpp"
#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "Nexus/Graphics/Mesh.hpp"
#include "Nexus/Graphics/Model.hpp"

namespace Nexus::Graphics
{
    /// @brief A class allowing a specific mesh to be generated or a model to be loaded from a file
    class MeshFactory
    {
    public:
        /// @brief A constructor to create a new MeshFactory
        /// @param device A graphics device to use to create resources
        explicit MeshFactory(GraphicsDevice *device)
        {
            m_Device = device;
        }

        /// @brief A method that returns a mesh representing a cube with a size of 1 unit in all directions
        /// @return A mesh representing a cube
        Mesh *CreateCube();

        /// @brief A method that returns a mesh representing a sprite with a width and height of 1
        /// @return A mesh representing a sprite
        Mesh *CreateSprite();

        /// @brief A method that returns a mesh representing a triangle with a width and height of 1
        /// @return A mesh representing a triangle
        Mesh *CreateTriangle();

        /// @brief A method that returns a mesh representing a 3D model stored on disk
        /// @param filepath The filepath to load a model from
        /// @return A mesh representing the model
        Nexus::Graphics::Model *CreateFrom3DModelFile(const std::string &filepath);

    private:
        /// @brief A pointer to a graphics device to use to create the vertex buffer and index buffer
        GraphicsDevice *m_Device = nullptr;
    };
}