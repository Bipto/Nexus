#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Mesh.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/Types.hpp"

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
    Ref<Mesh> CreateCube();

    /// @brief A method that returns a mesh representing a sprite with a width and height of 1
    /// @return A mesh representing a sprite
    Ref<Mesh> CreateSprite();

    /// @brief A method that returns a mesh representing a triangle with a width and height of 1
    /// @return A mesh representing a triangle
    Ref<Mesh> CreateTriangle();

    /// @brief A method that returns a mesh representing a 3D model stored on disk
    /// @param filepath The filepath to load a model from
    /// @return A mesh representing the model
    Ref<Model> CreateFrom3DModelFile(const std::string &filepath);

  private:
    /// @brief A pointer to a graphics device to use to create the vertex buffer and index buffer
    GraphicsDevice *m_Device = nullptr;
};
} // namespace Nexus::Graphics