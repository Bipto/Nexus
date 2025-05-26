#include "DeviceBufferOpenGL_WebGL.hpp"

namespace Nexus::Graphics
{
	DeviceBufferOpenGL::DeviceBufferOpenGL(const DeviceBufferDescription &desc) : m_BufferDescription(desc)
	{
		GLenum bufferUsage = GL::GetBufferUsage(desc);

		glCall(glGenBuffers(1, &m_BufferHandle));
		glCall(glBindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));
		glCall(glBufferData(GL_COPY_READ_BUFFER, m_BufferDescription.SizeInBytes, nullptr, bufferUsage));
	}

	DeviceBufferOpenGL::~DeviceBufferOpenGL()
	{
		glCall(glDeleteBuffers(1, &m_BufferHandle));
	}

	void DeviceBufferOpenGL::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must have been created with Upload access");

		glCall(glBindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));
		glCall(glBufferSubData(GL_COPY_READ_BUFFER, offset, size, data));
	}

	std::vector<char> DeviceBufferOpenGL::GetData(uint32_t offset, uint32_t size) const
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must have been created with Readback access");

		std::vector<char> data(size);
		glCall(glBindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));

		void *mappedData = glMapBufferRange(GL_COPY_READ_BUFFER, offset, size, GL_MAP_READ_BIT);
		if (mappedData)
		{
			memcpy(data.data(), mappedData, size);
		}

		glUnmapBuffer(GL_COPY_READ_BUFFER);

		return data;
	}

	const DeviceBufferDescription &DeviceBufferOpenGL::GetDescription() const
	{
		return m_BufferDescription;
	}

	void DeviceBufferOpenGL::MarkDirty()
	{
	}

	void DeviceBufferOpenGL::Bind(GLenum target)
	{
		glBindBuffer(target, m_BufferHandle);
	}

	void Nexus::Graphics::DeviceBufferOpenGL::BindRange(GLenum target, uint32_t slot, size_t offset, size_t size)
	{
		glBindBufferRange(target, slot, m_BufferHandle, offset, size);
	}
}	 // namespace Nexus::Graphics