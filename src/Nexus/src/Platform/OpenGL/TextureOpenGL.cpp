#if defined(NX_PLATFORM_OPENGL)

	#include "TextureOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	Texture2DOpenGL::Texture2DOpenGL(const Texture2DSpecification &spec, GraphicsDevice *graphicsDevice) : Texture2D(spec, graphicsDevice)
	{
		bool isDepth = false;

		if (Utils::Contains(spec.Usage, TextureUsage::DepthStencil))
		{
			isDepth = true;
		}

		m_DataFormat	 = GL::GetPixelDataFormat(spec.Format);
		m_InternalFormat = GL::GetSizedInternalFormat(spec.Format, isDepth);
		m_BaseType		 = GL::GetPixelType(spec.Format);

		glGenTextures(1, &m_Handle);
	#if defined(NX_PLATFORM_GL_DESKTOP)
		if (spec.Samples > 1)
		{
			m_TextureType	 = GL_TEXTURE_2D_MULTISAMPLE;
			glCall(glBindTexture(m_TextureType, m_Handle));
			glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
			glCall(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
											 spec.Samples,
											 m_InternalFormat,
											 m_Specification.Width,
											 m_Specification.Height,
											 GL_FALSE));
		}
		else
		{
	#endif
			// if we do not support multisampling, then we force this to 1
			m_Specification.Samples = 1;
			m_TextureType			= GL_TEXTURE_2D;
			glCall(glBindTexture(m_TextureType, m_Handle));
			glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
			glCall(glTexStorage2D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, spec.Width, spec.Height));

	#if defined(NX_PLATFORM_GL_DESKTOP)
		}
	#endif
	}

	Texture2DOpenGL::~Texture2DOpenGL()
	{
		glCall(glBindTexture(m_TextureType, m_Handle));
		glCall(glDeleteTextures(1, &this->m_Handle));

		if (m_Framebuffer)
		{
			glCall(glDeleteFramebuffers(1, &m_Framebuffer));
		}
	}

	void Texture2DOpenGL::SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glCall(glBindTexture(m_TextureType, m_Handle));
		glCall(glTexSubImage2D(m_TextureType, level, x, y, width, height, m_DataFormat, m_BaseType, data));
		glCall(glBindTexture(m_TextureType, m_Handle));
	}

	void Texture2DOpenGL::GetData(std::vector<unsigned char> &pixels, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		size_t sizeInBytes = GetPixelFormatSizeInBytes(m_Specification.Format);
		size_t bufferSize  = width * height * sizeInBytes;

		if (pixels.size() < bufferSize)
		{
			pixels.resize(bufferSize);
		}

		// create the framebuffer if it does not exist
		if (!m_Framebuffer)
		{
			glCall(glGenFramebuffers(1, &m_Framebuffer));
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
			glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureType, m_Handle, level));
		}

		if (m_Framebuffer)
		{
			float bottom = m_Specification.Height - height - y;
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
			glCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
			glCall(glReadPixels(x, bottom, width, height, m_DataFormat, m_BaseType, pixels.data()));
		}

		glCall(glBindTexture(m_TextureType, m_Handle));
	}

	unsigned int Texture2DOpenGL::GetHandle()
	{
		return m_Handle;
	}

	void Texture2DOpenGL::Bind(uint32_t slot)
	{
		glCall(glUniform1i(slot, slot));
		glCall(glActiveTexture(GL_TEXTURE0 + slot));
		glCall(glBindTexture(m_TextureType, m_Handle));
	}

	GLenum Texture2DOpenGL::GetTextureType()
	{
		return m_TextureType;
	}

	GLenum Texture2DOpenGL::GetDataFormat()
	{
		return m_DataFormat;
	}

	GLenum Texture2DOpenGL::GetPixelType()
	{
		return m_BaseType;
	}

	CubemapOpenGL::CubemapOpenGL(const CubemapSpecification &spec, GraphicsDevice *graphicsDevice) : Cubemap(spec, graphicsDevice)
	{
		m_DataFormat	 = GL::GetPixelDataFormat(spec.Format);
		m_InternalFormat = GL::GetSizedInternalFormat(spec.Format, false);
		m_BaseType		 = GL::GetPixelType(spec.Format);

		glCall(glGenTextures(1, &m_Handle));
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
		glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		for (size_t i = 0; i < 6; i++)
		{
			for (uint32_t level = 0; level < spec.MipLevels; level++)
			{
				glCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
									level,
									m_InternalFormat,
									m_Specification.Width,
									m_Specification.Height,
									0,
									m_DataFormat,
									m_BaseType,
									nullptr));
			}
		}
	}

	CubemapOpenGL::~CubemapOpenGL()
	{
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
		glCall(glDeleteTextures(1, &m_Handle));

		if (m_Framebuffer)
		{
			glCall(glDeleteFramebuffers(1, &m_Framebuffer));
		}
	}

	void CubemapOpenGL::SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
		GLenum glFace = GL::GLCubemapFace(face);
		glCall(glTexSubImage2D(glFace, level, x, y, width, height, m_DataFormat, m_BaseType, data));
	}

	void CubemapOpenGL::GetData(std::vector<unsigned char> &pixels,
								CubemapFace					face,
								uint32_t					level,
								uint32_t					x,
								uint32_t					y,
								uint32_t					width,
								uint32_t					height)
	{
		size_t bufferSize = (width - x) * (height - y) * GetPixelFormatSizeInBits(m_Specification.Format);
		GLenum glFace	  = GL::GLCubemapFace(face);

		if (pixels.size() < bufferSize)
		{
			pixels.resize(bufferSize);
		}

		if (!m_Framebuffer)
		{
			glCall(glGenFramebuffers(1, &m_Framebuffer));
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
			glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, glFace, GL_TEXTURE_2D, m_Handle, level));
		}

		glCall(glReadBuffer(glFace));
		glCall(glReadPixels(x, y, width, height, m_DataFormat, m_BaseType, pixels.data()));
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
	}

	unsigned int CubemapOpenGL::GetHandle()
	{
		return m_Handle;
	}

	void CubemapOpenGL::Bind(uint32_t slot)
	{
		glCall(glUniform1i(slot, slot));
		glCall(glActiveTexture(GL_TEXTURE0 + slot));
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
	}
}	 // namespace Nexus::Graphics

#endif