#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "Platform/OpenGL/GL.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class DeviceBufferOpenGL final : public IDeviceBuffer
	{
	  public:
		DeviceBufferOpenGL(const DeviceBufferDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~DeviceBufferOpenGL();
		void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		std::vector<char>			   GetData(uint32_t offset, uint32_t size) const final;
		const DeviceBufferDescription &GetDescription() const final;
		DeviceAddress				   GetDeviceAddress(size_t offset) const final;

		uint32_t GetHandle() const;

		void MarkDirty();

	  private:
		GraphicsDeviceOpenGL   *m_Device			= nullptr;
		DeviceBufferDescription m_BufferDescription = {};
		uint32_t				m_BufferHandle		= 0;
	};

}	 // namespace Nexus::Graphics

#endif