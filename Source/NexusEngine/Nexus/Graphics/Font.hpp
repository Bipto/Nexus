#pragma once

#include "GraphicsDevice.hpp"
#include "Texture.hpp"

#include <string>

namespace Nexus::Graphics
{
    struct FontData
    {
    public:
        FontData() = delete;
        FontData(uint32_t width, uint32_t height)
        {
            m_Width = width;
            m_Height = height;
            m_Pixels.resize(m_Width * m_Height);
        }

        std::vector<unsigned char> &GetPixels()
        {
            return m_Pixels;
        }

        void Clear(unsigned char value)
        {
            for (int i = 0; i < m_Pixels.size(); i++)
            {
                m_Pixels[i] = value;
            }
        }

        void SetPixel(uint32_t x, uint32_t y, unsigned char value)
        {
            x %= m_Width;
            y %= m_Height;

            y = m_Height - y - 1;

            uint32_t offset = x + (y * m_Width);
            m_Pixels[offset] = value;
        }

        operator std::vector<unsigned char>() const
        {
            return m_Pixels;
        }

    private:
        std::vector<unsigned char> m_Pixels;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };

    class Font
    {
    public:
        Font(const std::string &filepath, GraphicsDevice *device);
        Nexus::Graphics::Texture *GetTexture();

    private:
        std::vector<unsigned char> CreateBuffer(uint32_t width, uint32_t height);
        void Clear(std::vector<unsigned char> &buffer, unsigned char value);
        void SetPixel(std::vector<unsigned char> &buffer, uint32_t x, uint32_t y, uint32_t width, unsigned char value);

    private:
        Nexus::Graphics::Texture *m_Texture = nullptr;
    };
}