#include "DeviceBufferOpenGL_WebGL.hpp"

namespace Nexus::Graphics
{
	DeviceBufferOpenGL::DeviceBufferOpenGL(const DeviceBufferDescription &desc) : m_BufferDescription(desc)
	{
		std::vector<GLenum> targets		= GL::GetWebGLBufferTargets(desc.Usage);
		GLenum				bufferUsage = GL::GetBufferUsage(desc);
		for (GLenum target : targets)
		{
			WebGLBufferData bufferData = {};
			glCall(glGenBuffers(1, &bufferData.Handle));
			glCall(glBindBuffer(target, bufferData.Handle));
			glCall(glBufferData(target, m_BufferDescription.SizeInBytes, nullptr, bufferUsage));
			m_BufferHandles[target] = bufferData;
		}

		NX_ASSERT(targets.size() > 1, "Failed to create a buffer");
	}

	DeviceBufferOpenGL::~DeviceBufferOpenGL()
	{
		for (const auto &[type, bufferData] : m_BufferHandles) { glCall(glDeleteBuffers(1, &bufferData.Handle)); }
	}

	void DeviceBufferOpenGL::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must have been created with Upload access");

		auto [type, bufferData] = *m_BufferHandles.begin();
		glCall(glBindBuffer(type, bufferData.Handle));
		glCall(glBufferSubData(type, offset, size, data));

		MarkDirty();
	}

	std::vector<char> DeviceBufferOpenGL::GetData(uint32_t offset, uint32_t size) const
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must have been created with Readback access");

		auto [type, bufferData] = *m_BufferHandles.begin();
		std::vector<char> data(size);
		glCall(glBindBuffer(type, bufferData.Handle));

		void *mappedData = glMapBufferRange(type, offset, size, GL_MAP_READ_BIT);
		if (mappedData)
		{
			memcpy(data.data(), mappedData, size);
		}

		glUnmapBuffer(type);

		return data;
	}

	const DeviceBufferDescription &DeviceBufferOpenGL::GetDescription() const
	{
		return m_BufferDescription;
	}

	void DeviceBufferOpenGL::MarkDirty()
	{
		for (auto &[target, bufferData] : m_BufferHandles) { bufferData.Dirty = true; }
	}

	void DeviceBufferOpenGL::Bind(GLenum target)
	{
		if (target == GL_COPY_READ_BUFFER || target == GL_COPY_WRITE_BUFFER || target == GL_PIXEL_PACK_BUFFER || target == GL_PIXEL_UNPACK_BUFFER)
		{
			auto [type, bufferData] = *m_BufferHandles.begin();
			glBindBuffer(target, bufferData.Handle);
		}
		else
		{
			const WebGLBufferData &data = m_BufferHandles[target];
			glBindBuffer(target, data.Handle);
		}
	}

	void DeviceBufferOpenGL::BindRange(GLenum target, uint32_t slot, size_t offset, size_t size)
	{
		auto [type, bufferData] = *m_BufferHandles.begin();
		glBindBufferRange(target, slot, bufferData.Handle, offset, size);
	}
}	 // namespace Nexus::Graphics