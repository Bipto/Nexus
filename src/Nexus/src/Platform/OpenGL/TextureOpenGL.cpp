#if defined(NX_PLATFORM_OPENGL)

	#include "TextureOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

	#include <stb_image_write.h>

namespace Nexus::Graphics
{
	TextureOpenGL::TextureOpenGL(const TextureSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice) : Texture(spec)
	{
		NX_ASSERT(spec.ArrayLayers >= 1, "Texture must have at least one array layer");
		NX_ASSERT(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Usage & TextureUsage_Cubemap)
		{
			NX_ASSERT(spec.ArrayLayers % 6 == 0, "Cubemap texture must have 6 array layers");
		}

		if (spec.Samples > 1)
		{
			NX_ASSERT(spec.MipLevels == 0, "Multisampled textures do not support mipmapping");
		}

		bool isDepth			  = spec.Usage & Graphics::TextureUsage_DepthStencil;
		m_TextureType			  = GL::GetTextureType(spec);
		m_InternalFormat		  = GL::GetSizedInternalFormat(spec.Format, isDepth);
		m_BaseType				  = GL::GetPixelType(spec.Format);
		m_GLInternalTextureFormat = GL::GetGLInternalTextureFormat(spec);
		m_DataFormat			  = GL::GetPixelDataFormat(spec.Format);

		glCall(glGenTextures(1, &m_Handle));
		glCall(glBindTexture(m_TextureType, m_Handle));
		glCall(glPixelStorei(GL_PACK_ALIGNMENT, 1));
		glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		glCall(glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCall(glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		CreateTextureFaces();
	}

	TextureOpenGL::~TextureOpenGL()
	{
		glDeleteTextures(1, &m_Handle);
	}

	void TextureOpenGL::Bind(uint32_t slot)
	{
		glCall(glUniform1i(slot, slot));
		glCall(glActiveTexture(GL_TEXTURE0 + slot));
		glCall(glBindTexture(m_TextureType, m_Handle));
	}

	uint32_t TextureOpenGL::GetHandle()
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

	GLenum TextureOpenGL::GetBaseType()
	{
		return m_BaseType;
	}

	void TextureOpenGL::CreateTextureFaces()
	{
		switch (m_GLInternalTextureFormat)
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(glTexStorage1D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width));
				break;
	#else
				throw std::runtime_error("1D textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(glTexStorage2D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width, m_Specification.Height));
				break;
			case GL::GLInternalTextureFormat::Texture2DMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(glTexStorage2DMultisample(m_TextureType,
												 m_Specification.Samples,
												 m_InternalFormat,
												 m_Specification.Width,
												 m_Specification.Height,
												 GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
				glCall(glTexStorage3D(m_TextureType,
									  m_Specification.MipLevels,
									  m_InternalFormat,
									  m_Specification.Width,
									  m_Specification.Height,
									  m_Specification.ArrayLayers));
				break;
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(glTexStorage3DMultisample(m_TextureType,
												 m_Specification.Samples,
												 m_InternalFormat,
												 m_Specification.Width,
												 m_Specification.Height,
												 m_Specification.ArrayLayers,
												 GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
		}
	}

	GL::GLInternalTextureFormat TextureOpenGL::GetInternalGLTextureFormat() const
	{
		return m_GLInternalTextureFormat;
	}
}	 // namespace Nexus::Graphics

#endif