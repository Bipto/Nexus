#include "Font.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Nexus::Graphics
{
    Font::Font(const std::string &filepath, GraphicsDevice *device)
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "Could not initialise FreeType" << std::endl;
        }

        FT_Face face;
        if (FT_New_Face(ft, filepath.c_str(), 0, &face))
        {
            std::cout << "Failed to load font" << std::endl;
        }
        FT_Set_Pixel_Sizes(face, 0, 72);

        if (FT_Load_Char(face, 'a', FT_LOAD_RENDER))
        {
            std::cout << "Failed to load glyph" << std::endl;
        }

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.NumberOfChannels = 1;
        textureSpec.Format = Nexus::Graphics::TextureFormat::R8;
        textureSpec.Width = 512;
        textureSpec.Height = 512;

        FontData pixels(512, 512);
        pixels.Clear(0);

        for (int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            for (int x = 0; x < face->glyph->bitmap.width; x++)
            {
                unsigned char pixel = face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.width)];
                pixels.SetPixel(x, y, pixel);
            }
        }

        textureSpec.Data = pixels.GetPixels().data();
        m_Texture = device->CreateTexture(textureSpec);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    Nexus::Graphics::Texture *Font::GetTexture()
    {
        return m_Texture;
    }

    std::vector<unsigned char> Font::CreateBuffer(uint32_t width, uint32_t height)
    {
        return std::vector<unsigned char>(width * height);
    }

    void Font::Clear(std::vector<unsigned char> &buffer, unsigned char value)
    {
        for (int i = 0; i < buffer.size(); i++)
        {
            buffer[i] = value;
        }
    }

    void Font::SetPixel(std::vector<unsigned char> &buffer, uint32_t x, uint32_t y, uint32_t width, unsigned char value)
    {
        uint32_t offset = x + (y * width);
        buffer[offset] = value;
    }
}