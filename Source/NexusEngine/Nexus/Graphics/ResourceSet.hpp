#pragma once

#include <string>
#include <inttypes.h>
#include <vector>

#include "Nexus/Memory.hpp"
#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/Buffer.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing a binding of a texture within a shader
    struct TextureResourceBinding
    {
        /// @brief An integer representing which slot the texture should be bound to
        uint32_t Slot;

        /// @brief The name of the texture (must match the name within the shader as this is required by some APIs)
        std::string Name;
    };

    /// @brief A class representing a binding to a uniform buffer
    struct UniformResourceBinding
    {
        /// @brief The binding index of the uniform buffer
        uint32_t Binding;

        /// @brief The name of the uniform buffer
        std::string Name;

        /// @brief A reference counted pointer to a uniform buffer
        Ref<UniformBuffer> Buffer;
    };

    struct ResourceSetSpecification
    {
        std::vector<TextureResourceBinding> TextureBindings;
        std::vector<UniformResourceBinding> UniformResourceBindings;
    };

    class ResourceSet
    {
    public:
        ResourceSet(const ResourceSetSpecification &spec) : m_Specification(spec) {}
        virtual ~ResourceSet(){}

        virtual void WriteTexture(Ref<Texture> texture, uint32_t binding) = 0;
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding) = 0;

        const ResourceSetSpecification &GetSpecification() { return m_Specification; }

    protected:
        ResourceSetSpecification m_Specification;
    };
}