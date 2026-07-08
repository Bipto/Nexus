#pragma once
#include "RHI/RHI-Core.hpp"
#include "RHI/ShaderDataType.hpp"
#include "RHI/VertexBufferLayout.hpp"
#include <glm/glm.hpp>

namespace Nexus::Graphics
{

    /// @brief A struct representing a vertex with 3D position in world space
    struct VertexPosition
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        VertexPosition() = default;

        /// @brief A constructor taking in the position of the vertex
        /// @param position A const reference to 3 floating point values representing
        /// the position
        VertexPosition(const glm::vec3 &position) : Position(position)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
                sizeof(VertexPosition),
                StepRate::Vertex
            };

            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space and a
    /// texture coordinate
    struct VertexPositionTexCoord
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of
        /// the vertex
        glm::vec2 TexCoords = {0, 0};

        VertexPositionTexCoord() = default;

        /// @brief A constructor taking in the position of the vertex and its texture
        /// coordinate
        /// @param position A const reference to 3 floating point values representing
        /// the position
        /// @param texCoords A const reference to 2 floating point values
        /// representing the texture coordinates
        VertexPositionTexCoord(const glm::vec3 &position, const glm::vec2 &texCoords)
            : Position(position), TexCoords(texCoords)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"}},
                sizeof(VertexPositionTexCoord),
                StepRate::Vertex
            };
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space, a
    /// texture coordinate and a color
    struct VertexPositionTexCoordColor
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of
        /// the vertex
        glm::vec2 TexCoords = {0, 0};

        // @brief 4 floating point values representing the colour of the vertex
        glm::vec4 Colour = {1.0f, 1.0f, 1.0f, 1.0f};

        VertexPositionTexCoordColor() = default;

        /// @brief A constructor taking in the position of the vertex and its texture
        /// coordinate
        /// @param position A const reference to 3 floating point values representing
        /// the position
        /// @param texCoords A const reference to 2 floating point values
        /// representing the texture coordinates
        /// @param color A const reference to 4 floating point values repsenting the
        /// colour
        VertexPositionTexCoordColor(
            const glm::vec3 &position, const glm::vec2 &texCoords,
            const glm::vec4 &color
        )
            : Position(position), TexCoords(texCoords), Colour(color)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"}},
                sizeof(VertexPositionTexCoordColor),
                StepRate::Vertex
            };
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space, a
    /// texture coordinate and a normal
    struct VertexPositionTexCoordNormal
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of
        /// the vertex
        glm::vec2 TexCoords = {0, 0};

        /// @brief 3 floating point values representing the normal of the vertex
        glm::vec3 Normal = {0, 0, 0};

        VertexPositionTexCoordNormal() = default;

        /// @brief A constructor taking in the position of the vertex, it's texture
        /// coordinate and it's normal
        /// @param position A const reference to 3 floating point values representing
        /// the position
        /// @param texCoords A const reference to 2 floating point values
        /// representing the texture coordinates
        /// @param normal A const reference to 3 floating point values representing
        /// the normal
        VertexPositionTexCoordNormal(
            const glm::vec3 &position, const glm::vec2 &texCoords,
            const glm::vec3 &normal
        )
            : Position(position), TexCoords(texCoords), Normal(normal)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
                sizeof(VertexPositionTexCoordNormal),
                StepRate::Vertex
            };
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space, a
    /// texture coordinate, normal, tangent and binormal
    struct VertexPositionTexCoordNormalTangentBitangent
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of
        /// the vertex
        glm::vec2 TexCoords = {0, 0};

        /// @brief 3 floating point values representing the normal of the vertex
        glm::vec3 Normal = {0, 0, 0};

        /// @brief 3 floating point values representing the tangent of the vertex
        glm::vec3 Tangent = {0, 0, 0};

        /// @brief 3 floating point values representing the binormal of the vertex
        glm::vec3 Bitangent = {0, 0, 0};

        VertexPositionTexCoordNormalTangentBitangent() = default;

        /// @brief A constructor taking in the position of the vertex, it's texture
        /// coordinate and it's normal
        /// @param position A const reference to 3 floating point values representing
        /// the position
        /// @param texCoords A const reference to 2 floating point values
        /// representing the texture coordinates
        /// @param normal A const reference to 3 floating point values representing
        /// the normal
        VertexPositionTexCoordNormalTangentBitangent(
            const glm::vec3 &position, const glm::vec2 &texCoords,
            const glm::vec3 &normal, const glm::vec3 &tangent,
            const glm::vec3 &bitangent
        )
            : Position(position), TexCoords(texCoords), Normal(normal),
              Tangent(tangent), Bitangent(bitangent)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
                sizeof(VertexPositionTexCoordNormalTangentBitangent),
                StepRate::Vertex
            };
            return layout;
        }
    };

    /// @brief A struct representing a vertex with 3D position in world space, a
    /// texture coordinate, normal, tangent and binormal
    struct VertexPositionTexCoordNormalColourTangentBitangent
    {
        /// @brief 3 floating point values representing position of the vertex
        glm::vec3 Position = {0, 0, 0};

        /// @brief 2 floating point values representing the texture coordinates of
        /// the vertex
        glm::vec2 TexCoords = {0, 0};

        /// @brief 3 floating point values representing the normal of the vertex
        glm::vec3 Normal = {0, 0, 0};

        /// @brief  4 floating point values representing the colour of the vertex
        glm::vec4 Colour = {1, 1, 1, 1};

        /// @brief 3 floating point values representing the tangent of the vertex
        glm::vec3 Tangent = {0, 0, 0};

        /// @brief 3 floating point values representing the binormal of the vertex
        glm::vec3 Bitangent = {0, 0, 0};

        VertexPositionTexCoordNormalColourTangentBitangent() = default;

        /// @brief A constructor taking in the position of the vertex, it's texture
        /// coordinate and it's normal
        /// @param position A const reference to 3 floating point values representing
        /// the position
        /// @param texCoords A const reference to 2 floating point values
        /// representing the texture coordinates
        /// @param normal A const reference to 3 floating point values representing
        /// the normal
        VertexPositionTexCoordNormalColourTangentBitangent(
            const glm::vec3 &position, const glm::vec2 &texCoords,
            const glm::vec3 &normal, const glm::vec4 &colour,
            const glm::vec3 &tangent, const glm::vec3 &bitangent
        )
            : Position(position), TexCoords(texCoords), Normal(normal),
              Colour(colour), Tangent(tangent), Bitangent(bitangent)
        {
        }

        /// @brief A static method that returns the vertex buffer layout of this
        /// vertex type
        /// @return A VertexBufferLayout object containing the buffer layout of the
        /// type
        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout = {
                {{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
                 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
                sizeof(VertexPositionTexCoordNormalColourTangentBitangent),
                StepRate::Vertex
            };
            return layout;
        }
    };

    namespace Utilities
    {
        NX_RHI_API std::vector<VertexPositionTexCoordNormalTangentBitangent>
        GenerateTangentAndBinormals(
            const std::vector<Nexus::Graphics::VertexPositionTexCoordNormal>
                &vertices
        );
    }
} // namespace Nexus::Graphics