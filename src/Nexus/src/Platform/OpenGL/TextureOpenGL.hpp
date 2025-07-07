#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "DeviceBufferOpenGL.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class TextureOpenGL : public Texture
	{
	  public:
		TextureOpenGL(const TextureSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice);
		virtual ~TextureOpenGL();

		void	 Bind(uint32_t slot);
		uint32_t GetHandle();
		GLenum	 GetTextureType();
		GLenum	 GetDataFormat();
		GLenum	 GetBaseType();

		GL::GLInternalTextureFormat GetInternalGLTextureFormat() const;

	  private:
		void CreateTextureFacesDSA(const GladGLContext &context);
		void CreateTextureFacesNonDSA(const GladGLContext &context);

	  private:
		GraphicsDeviceOpenGL *m_Device = nullptr;

		uint32_t m_Handle		  = 0;
		GLenum	 m_DataFormat	  = 0;
		GLenum	 m_InternalFormat = 0;
		GLenum	 m_BaseType		  = 0;
		GLenum	 m_TextureType	  = 0;

		uint32_t					m_Framebuffer			  = 0;
		GL::GLInternalTextureFormat m_GLInternalTextureFormat = GL::GLInternalTextureFormat::Texture2D;
	};
}	 // namespace Nexus::Graphics

#endif