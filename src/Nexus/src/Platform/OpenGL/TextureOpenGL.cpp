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
		glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		glCall(glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCall(glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		CreateTextureFaces();
	}

	TextureOpenGL::~TextureOpenGL()
	{
		glCall(glDeleteTextures(1, &m_Handle));
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

	void TextureOpenGL::CreateTextureFaces()
	{
		switch (m_GLInternalTextureFormat)
		{
			case GL::GLInternalTextureFormat::Texture1D:
				glCall(glTexStorage1D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width));
				break;
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(glTexStorage2D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width, m_Specification.Height));
				break;
			case GL::GLInternalTextureFormat::Texture2DMultisample:
				glCall(glTexStorage2DMultisample(m_TextureType,
												 m_Specification.Samples,
												 m_InternalFormat,
												 m_Specification.Width,
												 m_Specification.Height,
												 GL_TRUE));
				break;
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
				glCall(glTexStorage3DMultisample(m_TextureType,
												 m_Specification.Samples,
												 m_InternalFormat,
												 m_Specification.Width,
												 m_Specification.Height,
												 m_Specification.ArrayLayers,
												 GL_TRUE));
				break;
		}
	}

	void TextureOpenGL::CopyDataFromBuffer(DeviceBufferOpenGL *buffer, uint32_t bufferOffset, SubresourceDescription subresource)
	{
		NX_ASSERT(m_Specification.Samples == 1, "Cannot set data in a multisampled texture");

		if (subresource.Depth > 1)
		{
			NX_ASSERT(m_Specification.Type == TextureType::Texture3D,
					  "Attempting to set data in a multi-layer texture, but texture is not multi layer");
		}

		glCall(glBindTexture(m_TextureType, m_Handle));
		glCall(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->GetBufferHandle()));

		GLenum	 glAspect = GL::GetGLImageAspect(subresource.Aspect);
		uint32_t bufferSize =
			(subresource.Width - subresource.X) * (subresource.Height - subresource.Y) * (uint32_t)GetPixelFormatSizeInBytes(m_Specification.Format);

		switch (m_GLInternalTextureFormat)
		{
			case GL::GLInternalTextureFormat::Texture1D:
				glCall(glTexSubImage1D(m_TextureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Width,
									   m_DataFormat,
									   m_BaseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Texture2DMultisample:
				glCall(glTexSubImage2D(m_TextureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.Width,
									   subresource.Height,
									   m_DataFormat,
									   m_BaseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + subresource.ArrayLayer,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.Width,
									   subresource.Height,
									   m_DataFormat,
									   m_BaseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
				glCall(glTexSubImage3D(m_TextureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.ArrayLayer,
									   subresource.Width,
									   subresource.Height,
									   subresource.Depth,
									   m_DataFormat,
									   m_BaseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
		}

		glCall(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
		glCall(glBindTexture(m_TextureType, 0));
	}

	void TextureOpenGL::CopyDataToBuffer(DeviceBufferOpenGL *buffer, uint32_t bufferOffset, SubresourceDescription subresource)
	{
		size_t layerSize =
			(subresource.Width - subresource.X) * (subresource.Height - subresource.Y) * GetPixelFormatSizeInBytes(m_Specification.Format);
		size_t bufferSize = layerSize * subresource.Depth;
		GLenum glAspect	  = GL::GetGLImageAspect(subresource.Aspect);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer->GetBufferHandle());

		for (uint32_t layer = subresource.Z; layer < subresource.Depth; layer++)
		{
			GLuint framebufferHandle = 0;
			glCall(glGenFramebuffers(1, &framebufferHandle));
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle));
			GL::AttachTexture(framebufferHandle, this, subresource.MipLevel, layer, subresource.Aspect, 0);

			GL::ValidateFramebuffer(framebufferHandle);

			glCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
			glCall(glReadPixels(subresource.X,
								subresource.Y,
								subresource.Width,
								subresource.Height,
								m_DataFormat,
								m_BaseType,
								(void *)(uint64_t)bufferOffset));

			glCall(glFlush());
			glCall(glFinish());
			glCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));

			glCall(glDeleteFramebuffers(1, &framebufferHandle));
			bufferOffset += layerSize;
		}

		glCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
		glCall(glBindTexture(m_TextureType, 0));
		glCall(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
	}

	GL::GLInternalTextureFormat TextureOpenGL::GetInternalGLTextureFormat() const
	{
		return m_GLInternalTextureFormat;
	}
}	 // namespace Nexus::Graphics

#endif