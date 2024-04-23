#pragma once

#include "Nexus/Types.hpp"
#include "PixelFormat.hpp"
#include "SamplerState.hpp"
#include "Multisample.hpp"

#include "Nexus/Buffer.hpp"

namespace Nexus::Graphics
{
    enum class TextureUsage : uint8_t
    {
        DepthStencil,
        RenderTarget,
        Sampled,
        Storage
    };

    /// @brief A struct that represents a set of values to use to create a texture
    struct TextureSpecification
    {
        /// @brief An unsigned 32 bit integer representing the width of the texture
        uint32_t Width = 512;

        /// @brief An unsigned 32 bit integer representing the height of the texture
        uint32_t Height = 512;

        /// @brief An unsigned 32 bit integer representing the number of mips in the texture
        uint32_t Levels = 1;

        /// @brief A value representing how many bits will be allocated for the texture
        PixelFormat Format = PixelFormat::R8_G8_B8_A8_UNorm;

        /// @brief A set of values to use for creating the texture
        std::vector<TextureUsage> Usage = {TextureUsage::Sampled};

        /// @brief How many samples should be used to sample a pixel
        SampleCount Samples = SampleCount::SampleCount1;
    };

    /// @brief A pure virtual class that represents an API specific texture
    class Texture
    {
    public:
        /// @brief A texture cannot be created without a texture specification
        Texture() = delete;

        /// @brief A constructor that takes in a texture specification
        /// @param spec The specification to create a texture from
        Texture(const TextureSpecification &spec)
            : m_Specification(spec){};

        virtual ~Texture() {}

        // virtual void SetData(const void *data, uint32_t size, uint32_t level) = 0;
        virtual void SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        const TextureSpecification &GetTextureSpecification() const { return m_Specification; }

    protected:
        /// @brief A specification describing the layout of the texture
        TextureSpecification m_Specification;
    };
}