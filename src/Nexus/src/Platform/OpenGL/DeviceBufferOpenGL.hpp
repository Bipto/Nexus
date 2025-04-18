#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
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

		uint32_t GetBufferHandle() const;
		uint32_t GetBufferTarget() const;

	  private:
		DeviceBufferDescription m_BufferDescription = {};
		uint32_t				m_BufferHandle		= 0;
		GLenum					m_BufferTarget		= 0;
	};
}	 // namespace Nexus::Graphics

#endif