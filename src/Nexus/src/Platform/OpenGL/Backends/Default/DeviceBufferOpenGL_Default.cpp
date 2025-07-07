#include "DeviceBufferOpenGL_Default.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	DeviceBufferOpenGL::DeviceBufferOpenGL(const DeviceBufferDescription &desc, GraphicsDeviceOpenGL *device)
		: m_Device(device),
		  m_BufferDescription(desc)
	{
		GLenum bufferUsage = GL::GetBufferUsage(desc);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_direct_state_access || context.EXT_direct_state_access)
				{
					glCall(context.CreateBuffers(1, &m_BufferHandle));
					glCall(context.NamedBufferData(m_BufferHandle, m_BufferDescription.SizeInBytes, nullptr, bufferUsage));
				}
				else
				{
					glCall(context.GenBuffers(1, &m_BufferHandle));
					glCall(context.BindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));
					glCall(context.BufferData(GL_COPY_READ_BUFFER, m_BufferDescription.SizeInBytes, nullptr, bufferUsage));
				}
			});
	}

	DeviceBufferOpenGL::~DeviceBufferOpenGL()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteBuffers(1, &m_BufferHandle); });
	}

	void DeviceBufferOpenGL::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must have been created with Upload access");

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_direct_state_access || context.EXT_direct_state_access)
				{
					glCall(context.NamedBufferSubData(m_BufferHandle, offset, size, data));
				}
				else
				{
					glCall(context.BindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));
					glCall(context.BufferSubData(GL_COPY_READ_BUFFER, offset, size, data));
				}
			});
	}

	std::vector<char> DeviceBufferOpenGL::GetData(uint32_t offset, uint32_t size) const
	{
		NX_ASSERT(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must have been created with Readback access");

		std::vector<char> data(size);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_direct_state_access || context.EXT_direct_state_access)
				{
					glCall(context.GetNamedBufferSubData(m_BufferHandle, offset, size, data.data()));
				}
				else
				{
					glCall(context.BindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));

					void *mappedData = context.MapBufferRange(GL_COPY_READ_BUFFER, offset, size, GL_MAP_READ_BIT);
					if (mappedData)
					{
						memcpy(data.data(), mappedData, size);
					}

					context.UnmapBuffer(GL_COPY_READ_BUFFER);
				}
			});

		return data;
	}

	const DeviceBufferDescription &DeviceBufferOpenGL::GetDescription() const
	{
		return m_BufferDescription;
	}

	uint32_t DeviceBufferOpenGL::GetHandle() const
	{
		return m_BufferHandle;
	}

	void DeviceBufferOpenGL::MarkDirty()
	{
	}
}	 // namespace Nexus::Graphics
