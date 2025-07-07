#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Platform/OpenGL/GL.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class DeviceBufferOpenGL : public DeviceBuffer
	{
	  public:
		DeviceBufferOpenGL(const DeviceBufferDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~DeviceBufferOpenGL();
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size) const final;
		virtual const DeviceBufferDescription &GetDescription() const final;

		uint32_t GetHandle() const;

		void MarkDirty();

	  private:
		GraphicsDeviceOpenGL   *m_Device			= nullptr;
		DeviceBufferDescription m_BufferDescription = {};
		uint32_t				m_BufferHandle		= 0;
	};

}	 // namespace Nexus::Graphics

#endif