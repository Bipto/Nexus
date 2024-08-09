#if defined(NX_PLATFORM_OPENGL)

#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec, GraphicsDevice *graphicsDevice)
        : Texture(spec, graphicsDevice)
    {
        bool isDepth = false;

        if (std::find(spec.Usage.begin(), spec.Usage.end(), Nexus::Graphics::TextureUsage::DepthStencil) != spec.Usage.end())
        {
            isDepth = true;
        }

        m_DataFormat = GL::GetPixelDataFormat(spec.Format);
        m_InternalFormat = GL::GetSizedInternalFormat(spec.Format, isDepth);
        m_BaseType = GL::GetPixelType(spec.Format);

        glGenTextures(1, &m_Handle);
#if defined(NX_PLATFORM_GL_DESKTOP)
        if (spec.Samples != SampleCount::SampleCount1)
        {
            uint32_t samples = GetSampleCount(spec.Samples);
            m_TextureType = GL_TEXTURE_2D_MULTISAMPLE;
            glCall(glBindTexture(m_TextureType, m_Handle));
            glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            glCall(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, m_InternalFormat, m_Specification.Width, m_Specification.Height, GL_FALSE));
        }
        else
        {
#endif
            // if we do not support multisampling, then we force this to 1
            m_Specification.Samples = SampleCount::SampleCount1;
            m_TextureType = GL_TEXTURE_2D;
            glCall(glBindTexture(m_TextureType, m_Handle));
            glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            glCall(glTexStorage2D(m_TextureType, m_Specification.Levels, m_InternalFormat, spec.Width, spec.Height));

#if defined(NX_PLATFORM_GL_DESKTOP)
        }
#endif
    }

    TextureOpenGL::~TextureOpenGL()
    {
        glCall(glDeleteTextures(1, &this->m_Handle));
    }

    void TextureOpenGL::SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glCall(glBindTexture(m_TextureType, m_Handle));
        glCall(glTexSubImage2D(m_TextureType, level, x, y, width, height, m_DataFormat, m_BaseType, data));
    }

    std::vector<std::byte> TextureOpenGL::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        size_t bufferSize = (width - x) * (height - y) * GetPixelFormatSizeInBits(m_Specification.Format);
        std::vector<std::byte> data(bufferSize);

        // OpenGL only allows pixel data to be read from a framebuffer so we create a temporary one
        uint32_t framebufferId = 0;
        glCall(glGenFramebuffers(1, &framebufferId));
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));

        glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Handle, level));

        glCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
        glCall(glReadPixels(x, y, width, height, m_DataFormat, m_BaseType, data.data()));

        // clean up framebuffer
        glCall(glDeleteFramebuffers(1, &framebufferId));

        return data;
    }

    unsigned int TextureOpenGL::GetNativeHandle()
    {
        return m_Handle;
    }

    GLenum TextureOpenGL::GetTextureType()
    {
        return m_TextureType;
    }

    GLenum TextureOpenGL::GetDataFormat()
    {
        return m_DataFormat;
    }

    GLenum TextureOpenGL::GetPixelType()
    {
        return m_BaseType;
    }
}

#endif