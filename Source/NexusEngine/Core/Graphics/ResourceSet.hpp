#pragma once

#include <string>
#include <inttypes.h>
#include <vector>

#include "Core/Memory.hpp"
#include "Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing a binding of a texture within a shader
    struct TextureResourceBinding
    {
        /// @brief An integer representing which slot the texture should be bound to
        int Slot;

        /// @brief The name of the texture (must match the name within the shader as this is required by some APIs)
        std::string Name;
    };

    /// @brief A class representing a binding to a uniform buffer
    struct UniformResourceBinding
    {
        /// @brief The total size of the uniform buffer in bytes
        uint32_t Size;

        /// @brief The binding index of the uniform buffer
        uint32_t Binding;

        /// @brief The name of the uniform buffer
        std::string Name;
    };

    struct ResourceSetSpecification
    {
        std::vector<TextureResourceBinding> TextureBindings;
        std::vector<UniformResourceBinding> UniformResourceBindings;
    };

    class ResourceSet
    {
    public:
        virtual void UpdateTexture(Ref<Texture> texture, uint32_t binding) = 0;
    };
}