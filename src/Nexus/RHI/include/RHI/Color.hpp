#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    /// @brief A class that stores a four component colour value
    struct Colour
    {
      public:
        /// @brief A constructor that takes in a red, green, blue and alpha channel
        /// @param r The red channel
        /// @param g The green channel
        /// @param b The blue channel
        /// @param a The alpha channel
        Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            m_Colour = a;
            m_Colour |= (b << 8);
            m_Colour |= (g << 16);
            m_Colour |= (r << 24);
        }

        /// @brief A method returning the red channel
        /// @return An 8 bit unsigned int containing the value of the red channel
        uint8_t GetRedChannel() const
        {
            return uint8_t(m_Colour >> 24);
        }

        /// @brief A method returning the green channel
        /// @return An 8 bit unsigned int containing the value of the green channel
        uint8_t GetGreenChannel() const
        {
            return uint8_t(m_Colour >> 16);
        }

        /// @brief A method returning the blue channel
        /// @return An 8 bit unsigned int containing the value of the blue channel
        uint8_t GetBlueChannel() const
        {
            return uint8_t(m_Colour >> 8);
        }

        /// @brief A method returning the alpha channel
        /// @return An 8 bit unsigned int containing the value of the alpha channel
        uint8_t GetAlphaChannel() const
        {
            return uint8_t(m_Colour);
        }

        /// @brief A method returning a 32 bit value representing the colour
        /// @return A 32 bit unsigned int containing the value of the colour
        uint32_t GetColor() const
        {
            return m_Colour;
        }

      private:
        /// @brief A variable containing a 32 bit unsigned int containing the colour
        uint32_t m_Colour = 0;
    };
} // namespace Nexus::Graphics