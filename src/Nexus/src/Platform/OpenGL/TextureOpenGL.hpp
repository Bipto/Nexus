#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
class Texture2DOpenGL : public Texture2D
{
  public:
    Texture2DOpenGL(const Texture2DSpecification &spec, GraphicsDevice *graphicsDevice);
    ~Texture2DOpenGL();
    virtual void SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    void Bind(uint32_t slot);

    unsigned int GetHandle();
    GLenum GetTextureType();
    GLenum GetDataFormat();
    GLenum GetPixelType();

  private:
    unsigned int m_Handle = 0;
    GLenum m_DataFormat = 0;
    GLenum m_InternalFormat = 0;
    GLenum m_BaseType = 0;
    GLenum m_TextureType = 0;
};

class CubemapOpenGL : public Cubemap
{
  public:
    CubemapOpenGL(const CubemapSpecification &spec, GraphicsDevice *graphicsDevice);
    ~CubemapOpenGL();
    virtual void SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    virtual std::vector<std::byte> GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    unsigned int GetHandle();
    void Bind(uint32_t slot);

  private:
    unsigned int m_Handle = 0;
    GLenum m_DataFormat = 0;
    GLenum m_InternalFormat = 0;
    GLenum m_BaseType = 0;
};
} // namespace Nexus::Graphics

#endif