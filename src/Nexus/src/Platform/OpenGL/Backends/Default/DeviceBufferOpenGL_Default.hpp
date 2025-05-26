#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Platform/OpenGL/GL.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	class DeviceBufferOpenGL : public DeviceBuffer
	{
	  public:
		DeviceBufferOpenGL(const DeviceBufferDescription &desc);
		virtual ~DeviceBufferOpenGL();
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size) const final;
		virtual const DeviceBufferDescription &GetDescription() const final;

		void Bind(GLenum target);
		void BindRange(GLenum target, uint32_t slot, size_t offset, size_t size);
		void MarkDirty();

	  private:
		DeviceBufferDescription m_BufferDescription = {};
		uint32_t				m_BufferHandle		= 0;
	};

}	 // namespace Nexus::Graphics

#endif