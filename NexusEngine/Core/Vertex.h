#pragma once
#include "glm/glm.hpp"

/// @brief A struct representing a vertex
struct Vertex
{
    /// @brief Three floating point values representing the position
    glm::vec3 Position;

    /// @brief Two floating point values representing the texture coordinates
    glm::vec2 TexCoords;
};