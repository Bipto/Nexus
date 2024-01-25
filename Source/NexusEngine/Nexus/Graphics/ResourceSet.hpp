#pragma once

#include <string>
#include <inttypes.h>
#include <vector>

#include "Nexus/Types.hpp"
#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/Buffer.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing a binding of a texture within a shader
    struct TextureResourceBinding
    {
        uint32_t Set;

        /// @brief An integer representing which slot the texture should be bound to
        uint32_t Slot;

        /// @brief The name of the texture (must match the name within the shader as this is required by some APIs)
        std::string Name;
    };

    /// @brief A class representing a binding to a uniform buffer
    struct UniformResourceBinding
    {
        uint32_t Set;

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
        ResourceSet(const ResourceSetSpecification &spec);
        virtual ~ResourceSet() {}

        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) = 0;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) = 0;

        const ResourceSetSpecification &GetSpecification() const;

        bool HasTextures() const;
        bool HasUniformBuffers() const;

        uint32_t GetTextureDescriptorIndex() const;
        uint32_t GetUniformBufferDescriptorIndex() const;

        static constexpr uint32_t DescriptorSlotCount = 16;
        static uint32_t GetLinearDescriptorSlot(uint32_t set, uint32_t slot);

    protected:
        ResourceSetSpecification m_Specification;

        uint32_t m_TextureDescriptorIndex = 0;
        uint32_t m_UniformBufferDescriptorIndex = 0;
    };
}