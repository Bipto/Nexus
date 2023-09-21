#pragma once

#include "TextureFormat.hpp"

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
        int32_t Width;

        /// @brief An unsigned 32 bit integer representing the height of the texture
        int32_t Height;

        /// @brief An unsigned 32 bit integer representing the number of channels in the texture
        int32_t NumberOfChannels;

        /// @brief A value representing how many bits will be allocated for the texture
        TextureFormat Format;

        /// @brief A value representing how the texture will be sampled
        SamplerState SamplerState;

        /// @brief A raw pointer to the memory to upload to the texture
        unsigned char *Data;

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
        Texture(const TextureSpecification &spec){};

        virtual ~Texture() {}

        /// @brief A pure virtual method that returns a pointer to the underlying native texture
        /// @return A void pointer to an API specific texture
        virtual void *GetHandle() = 0;

    protected:
        /// @brief An integer representing the width of the texture
        int m_Width;

        /// @brief An integer representing the height of the texture
        int m_Height;

        /// @brief An integer representing the number of channels in the texture
        int m_NumOfChannels;

        /// @brief An unsigned char pointer to the underlying data of the texture if retaining image data
        unsigned char *m_Data;
    };
}