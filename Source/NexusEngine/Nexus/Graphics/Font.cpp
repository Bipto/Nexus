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
        FT_Set_Pixel_Sizes(face, 0, 60);

        if (FT_Load_Char(face, 'B', FT_LOAD_RENDER))
        {
            std::cout << "Failed to load glyph" << std::endl;
        }

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.Width = face->glyph->bitmap.width;
        textureSpec.Height = face->glyph->bitmap.rows;
        textureSpec.NumberOfChannels = 1;
        textureSpec.Format = Nexus::Graphics::TextureFormat::RedInteger;
        textureSpec.Data = face->glyph->bitmap.buffer;

        m_Texture = device->CreateTexture(textureSpec);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    Nexus::Graphics::Texture *Font::GetTexture()
    {
        return m_Texture;
    }
}