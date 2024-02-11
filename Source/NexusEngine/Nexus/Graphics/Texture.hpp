#pragma once

#include "TextureFormat.hpp"
#include "Nexus/Types.hpp"

namespace Nexus::Graphics
{
    /// @brief An enum representing a method of sampling a texture
    enum SamplerState
    {
        /// @brief Pixels will be linearly interpolated and pixels out of range will be clamped to the nearest value in the texture
        LinearClamp,

        /// @brief Pixels will be linearly interpolated and pixels out of range will be wrapped around into the texture
        LinearWrap,

        /// @brief Pixels will be clamped to the nearest value and pixels out of range will be clamped to the nearest value in the texture
        PointClamp,

        /// @brief Pixels will be clamped to the nearest value and pixels out of range will be wrapped around into the texture
        PointWrap
    };

    /// @brief A struct that represents a set of values to use to create a texture
    struct TextureSpecification
    {
        /// @brief An unsigned 32 bit integer representing the width of the texture
        int32_t Width = 512;

        /// @brief An unsigned 32 bit integer representing the height of the texture
        int32_t Height = 512;

        /// @brief An unsigned 32 bit integer representing the number of channels in the texture
        int32_t NumberOfChannels = 4;

        /// @brief A value representing how many bits will be allocated for the texture
        TextureFormat Format = TextureFormat::RGBA8;

        /// @brief A value representing how the texture will be sampled
        SamplerState SamplerState = SamplerState::LinearWrap;

        /// @brief Whether image data will be retained in RAM
        bool RetainImageData = true;
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

        /// @brief A pure virtual method that returns a pointer to the underlying native texture
        /// @return A void pointer to an API specific texture
        virtual ResourceHandle GetHandle() = 0;

        virtual void SetData(const void *data, uint32_t size) = 0;

    protected:
        /// @brief An unsigned char pointer to the underlying data of the texture if retaining image data
        unsigned char *m_Data = nullptr;

        /// @brief A specification describing the layout of the texture
        TextureSpecification m_Specification;
    };
}