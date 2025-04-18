#include "DeviceBufferOpenGL.hpp"

namespace Nexus::Graphics
{
	DeviceBufferOpenGL::DeviceBufferOpenGL(const DeviceBufferDescription &desc) : m_BufferDescription(desc)
	{
		m_BufferTarget	   = GL::GetBufferTarget(desc.Type);
		GLenum bufferUsage = GL::GetBufferUsage(desc.Type, desc.HostAccessible);

		glCall(glGenBuffers(1, &m_BufferHandle));
		glCall(glBindBuffer(m_BufferTarget, m_BufferHandle));
		glCall(glBufferData(m_BufferTarget, m_BufferDescription.SizeInBytes, nullptr, bufferUsage));
	}

	DeviceBufferOpenGL::~DeviceBufferOpenGL()
	{
		glCall(glDeleteBuffers(1, &m_BufferHandle));
	}

	void DeviceBufferOpenGL::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.HostAccessible == true, "Cannot access a non-host accessible buffer");

		glCall(glBindBuffer(m_BufferTarget, m_BufferHandle));
		glCall(glBufferSubData(m_BufferTarget, offset, size, data));
	}

	std::vector<char> DeviceBufferOpenGL::GetData(uint32_t offset, uint32_t size) const
	{
		NX_ASSERT(m_BufferDescription.HostAccessible == true, "Cannot access a non-host accessible buffer");

		std::vector<char> data(size);
		glCall(glBindBuffer(m_BufferTarget, m_BufferHandle));

		void *mappedData = glMapBufferRange(m_BufferTarget, offset, size, GL_MAP_READ_BIT);
		if (mappedData)
		{
			memcpy(data.data(), mappedData, size);
			glUnmapBuffer(m_BufferTarget);
		}

		return data;
	}

	const DeviceBufferDescription &DeviceBufferOpenGL::GetDescription() const
	{
		return m_BufferDescription;
	}

	uint32_t DeviceBufferOpenGL::GetBufferHandle() const
	{
		return m_BufferHandle;
	}

	uint32_t DeviceBufferOpenGL::GetBufferTarget() const
	{
		return m_BufferTarget;
	}
}	 // namespace Nexus::Graphics