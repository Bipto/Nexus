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
		if (spec.Samples != SampleCount::SampleCount1)
		{
			uint32_t samples = GetSampleCount(spec.Samples);
			m_TextureType	 = GL_TEXTURE_2D_MULTISAMPLE;
			glCall(glBindTexture(m_TextureType, m_Handle));
			glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
			glCall(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
											 samples,
											 m_InternalFormat,
											 m_Specification.Width,
											 m_Specification.Height,
											 GL_FALSE));
		}
		else
		{
	#endif
			// if we do not support multisampling, then we force this to 1
			m_Specification.Samples = SampleCount::SampleCount1;
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
		glCall(glDeleteTextures(1, &this->m_Handle));
	}

	void Texture2DOpenGL::SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glCall(glBindTexture(m_TextureType, m_Handle));
		glCall(glTexSubImage2D(m_TextureType, level, x, y, width, height, m_DataFormat, m_BaseType, data));
	}

	std::vector<std::byte> Texture2DOpenGL::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		size_t				   bufferSize = (width - x) * (height - y) * GetPixelFormatSizeInBits(m_Specification.Format);
		std::vector<std::byte> data(bufferSize);

		// OpenGL only allows pixel data to be read from a framebuffer so we create a
		// temporary one
		uint32_t framebufferId = 0;
		glCall(glGenFramebuffers(1, &framebufferId));
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));

		glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureType, m_Handle, level));
		glCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
		glCall(glReadPixels(x, y, width, height, m_DataFormat, m_BaseType, data.data()));

		// clean up framebuffer
		glCall(glDeleteFramebuffers(1, &framebufferId));

		return data;
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
		glCall(glDeleteTextures(1, &m_Handle));
	}

	void CubemapOpenGL::SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle));
		GLenum glFace = GL::GLCubemapFace(face);
		glCall(glTexSubImage2D(glFace, level, x, y, width, height, m_DataFormat, m_BaseType, data));
	}

	std::vector<std::byte> CubemapOpenGL::GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		GLenum				   glFace	  = GL::GLCubemapFace(face);
		size_t				   bufferSize = (width - x) * (height - y) * GetPixelFormatSizeInBits(m_Specification.Format);
		std::vector<std::byte> data(bufferSize);

		uint32_t framebufferId = 0;
		glCall(glGenFramebuffers(1, &framebufferId));
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferId));

		glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, glFace, GL_TEXTURE_2D, m_Handle, level));

		glCall(glReadBuffer(glFace));
		glCall(glReadPixels(x, y, width, height, m_DataFormat, m_BaseType, data.data()));

		glCall(glDeleteFramebuffers(1, &framebufferId));

		return data;
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