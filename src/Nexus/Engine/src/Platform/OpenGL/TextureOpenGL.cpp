#if defined(NX_PLATFORM_OPENGL)

#include "TextureOpenGL.hpp"

#include "GL.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

#include "GraphicsDeviceOpenGL.hpp"
#include "TextureViewOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureDescription &spec, GraphicsDeviceOpenGL *graphicsDevice)
        : ITexture(spec), m_Device(graphicsDevice)
    {
        NX_VALIDATE(spec.DepthOrArrayLayers >= 1, "Texture must have at least one array layer");
        NX_VALIDATE(spec.MipLevels >= 1, "Texture must have at least one mip level");

        if (spec.Samples > 1)
        {
            NX_VALIDATE(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
        }

        if (spec.Type == TextureType::TextureCube)
        {
            NX_VALIDATE(spec.DepthOrArrayLayers % 6 == 0, "Cubemap textures must have a multiple of 6 faces");
        }

        m_TextureType = GL::GetTextureType(spec);
        m_InternalFormat = GL::GetSizedInternalFormat(spec.Format);
        m_BaseType = GL::GetPixelType(spec.Format);
        m_GLInternalTextureFormat = GL::GetGLInternalTextureFormat(spec);
        m_DataFormat = GL::GetPixelDataFormat(spec.Format);

        GL::IGLContext *context = m_Device->GetOffscreenContext();
        CreateTextureFaces(context);
        context->ObjectLabel(GL_TEXTURE, m_Handle, -1, m_Description.DebugName.c_str());

        m_TextureLayout.resize(spec.DepthOrArrayLayers * spec.MipLevels, TextureLayout::Undefined);
    }

    TextureOpenGL::~TextureOpenGL()
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->Execute([&](const GladGLContext &context) { context.DeleteTextures(1, &m_Handle); });
    }

    TextureLayout TextureOpenGL::GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const
    {
        NX_VALIDATE(arrayLayer < m_Description.DepthOrArrayLayers, "Array layer out of bounds");
        NX_VALIDATE(mipLevel < m_Description.MipLevels, "Mip level out of bounds");

        size_t index = (size_t)(mipLevel + arrayLayer * m_Description.MipLevels);
        return m_TextureLayout.at(index);
    }

    void TextureOpenGL::SetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout)
    {
        NX_VALIDATE(arrayLayer < m_Description.DepthOrArrayLayers, "Array layer out of bounds");
        NX_VALIDATE(mipLevel < m_Description.MipLevels, "Mip level out of bounds");

        size_t index = (size_t)(mipLevel + arrayLayer * m_Description.MipLevels);
        m_TextureLayout[index] = layout;
    }

    SubresourceFootprint TextureOpenGL::GetSubresourceFootprint(uint32_t arrayLayer, uint32_t mipLevel) const
    {
        SubresourceFootprint footprint = {};
        size_t pixelSize = GetPixelFormatSizeInBytes(m_Description.Format);

        GLint readbackAlignment = 0;
        GLint uploadAlignment = 0;
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->Execute([&](const GladGLContext &context) {
            context.GetIntegerv(GL_PACK_ALIGNMENT, &readbackAlignment);
            context.GetIntegerv(GL_UNPACK_ALIGNMENT, &uploadAlignment);

            NX_ASSERT(readbackAlignment == uploadAlignment, "Mismatch between upload and readback alignment");
        });

        size_t alignedPixelSize = Utils::AlignTo<size_t>(pixelSize, readbackAlignment);
        Point2D<uint32_t> mipSize = Utils::GetMipSize(m_Description.Width, m_Description.Height, mipLevel);

        footprint.Size = static_cast<size_t>(mipSize.X) * static_cast<size_t>(mipSize.Y) * alignedPixelSize;
        footprint.RowPitch = static_cast<size_t>(mipSize.X) * alignedPixelSize;
        footprint.RowCount = mipSize.Y;

        return footprint;
    }

    void TextureOpenGL::Bind(uint32_t slot) const
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->Execute([&](const GladGLContext &context) {
            if (context.ARB_direct_state_access || context.EXT_direct_state_access)
            {
                glCall(context.BindTextureUnit(slot, m_Handle));
            }
            else
            {
                glCall(context.ActiveTexture(GL_TEXTURE0 + slot));
                glCall(context.BindTexture(m_TextureType, m_Handle));
            }
        });
    }

    uint32_t TextureOpenGL::GetHandle() const
    {
        return m_Handle;
    }

    GLenum TextureOpenGL::GetTextureType() const
    {
        return m_TextureType;
    }

    GLenum TextureOpenGL::GetDataFormat() const
    {
        return m_DataFormat;
    }

    GLenum TextureOpenGL::GetBaseType() const
    {
        return m_BaseType;
    }

    void TextureOpenGL::CreateTextureFaces(GL::IGLContext *context)
    {
        m_Handle = context->CreateTexture(m_TextureType).value();

        if (m_Description.CreateFlags & Graphics::TextureCreateFlags_SparseBinding)
        {
            NX_VALIDATE(
                context->IsSparseBindingSupported(), "Context must support the ARB_sparse_texture extension to use "
                                                     "sparse textures"
            );
            context->TextureParameteri(m_Handle, m_TextureType, GL_TEXTURE_SPARSE_ARB, GL_TRUE);
        }

        switch (m_GLInternalTextureFormat)
        {
        case GL::GLInternalTextureFormat::Texture1D:
        {
            NX_VALIDATE(
                context->IsTextureTypeSupported(TextureType::Texture1D, m_Description.Samples),
                "1D textures are not supported"
            );
            context->TexStorage1D(
                m_Handle, m_TextureType, m_Description.MipLevels, m_InternalFormat, m_Description.Width
            );
            break;
        }
        case GL::GLInternalTextureFormat::Texture1DArray:
        case GL::GLInternalTextureFormat::Texture2D:
        case GL::GLInternalTextureFormat::Cubemap:
        {
            NX_VALIDATE(
                context->IsTextureTypeSupported(TextureType::Texture2D, m_Description.Samples),
                "2D textures are not supported"
            );
            context->TexStorage2D(
                m_Handle, m_TextureType, m_Description.MipLevels, m_InternalFormat, m_Description.Width,
                m_Description.Height
            );
            break;
        }
        case GL::GLInternalTextureFormat::Texture2DMultisample:
        {
            NX_VALIDATE(
                context->IsTextureTypeSupported(TextureType::Texture2D, m_Description.Samples),
                "Multisampled 2D textures are not supported"
            );
            context->TexStorage2DMultisample(
                m_Handle, m_TextureType, m_Description.Samples, m_InternalFormat, m_Description.Width,
                m_Description.Height, GL_TRUE
            );
            break;
        }
        case GL::GLInternalTextureFormat::Texture2DArray:
        case GL::GLInternalTextureFormat::CubemapArray:
        case GL::GLInternalTextureFormat::Texture3D:
        {
            NX_VALIDATE(
                context->IsTextureTypeSupported(TextureType::Texture3D, m_Description.Samples),
                "3D textures are not supported"
            );
            context->TexStorage3D(
                m_Handle, m_TextureType, m_Description.MipLevels, m_InternalFormat, m_Description.Width,
                m_Description.Height, m_Description.DepthOrArrayLayers
            );
            break;
        }

        case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
        {
            NX_VALIDATE(
                context->IsTextureTypeSupported(TextureType::Texture3D, m_Description.Samples),
                "Multisampled 3D textures are not supported"
            );
            context->TexStorage3DMultisample(
                m_Handle, m_TextureType, m_Description.Samples, m_InternalFormat, m_Description.Width,
                m_Description.Height, m_Description.DepthOrArrayLayers, GL_TRUE
            );
            break;
        }
        }
    }

    GL::GLInternalTextureFormat TextureOpenGL::GetInternalGLTextureFormat() const
    {
        return m_GLInternalTextureFormat;
    }

    void TextureOpenGL::AddView(TextureViewHandle view)
    {
        m_Views.push_back(view);
    }

    void TextureOpenGL::MarkDirty()
    {
        for (TextureViewHandle view : m_Views)
        {
            TextureViewOpenGL *glView = view.AsDerived<TextureViewOpenGL>();
            if (view.IsValid() && glView)
            {
                glView->MarkDirty();
            }
        }
    }
} // namespace Nexus::Graphics

#endif