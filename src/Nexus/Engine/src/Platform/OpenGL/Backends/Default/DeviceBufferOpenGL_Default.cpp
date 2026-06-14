#include "DeviceBufferOpenGL_Default.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{

	DeviceBufferOpenGL::DeviceBufferOpenGL(const DeviceBufferDescription &desc, GraphicsDeviceOpenGL *device)
		: m_Device(device),
		  m_BufferDescription(desc)
	{
		const GLenum	 bufferUsage = GL::GetBufferUsage(desc);
		const GLbitfield mapFlags	 = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		// create buffer (try to use BufferStorage functions if available, otherwise fall back to using BufferData and a vector of CPU data for
		// mapping)
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->CreateBuffer(m_BufferHandle,
							  GL_COPY_READ_BUFFER,
							  desc.SizeInBytes,
							  nullptr,
							  mapFlags,
							  bufferUsage,
							  desc.DebugName,
							  m_PersistentMapping);
	}

	DeviceBufferOpenGL::~DeviceBufferOpenGL()
	{
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->DeleteBuffers(1, &m_BufferHandle);
	}

	void DeviceBufferOpenGL::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must have been created with Upload access");

		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->BufferSubData(m_BufferHandle, GL_COPY_READ_BUFFER, offset, size, data);
	}

	std::vector<char> DeviceBufferOpenGL::GetData(uint32_t offset, uint32_t size)
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must have been created with Readback access");

		std::vector<char> data(size);

		GL::IGLContext *context = m_Device->GetOffscreenContext();

		context->Execute(
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
	{ return m_BufferDescription; }

	DeviceAddress DeviceBufferOpenGL::GetDeviceAddress(size_t offset) const
	{ return 0 + offset; }

	uint8_t *DeviceBufferOpenGL::Map()
	{
		if (m_PersistentMapping)
		{
			GL::IGLContext *context = m_Device->GetOffscreenContext();

			context->Execute(
				[&](const GladGLContext &context)
				{
					if (context.ARB_direct_state_access || context.EXT_direct_state_access)
					{
						return context.MapNamedBufferRange(m_BufferHandle, 0, m_BufferDescription.SizeInBytes, mapFlags);
					}
					else
					{
						glCall(context.BindBuffer(GL_COPY_READ_BUFFER, m_BufferHandle));
						return context.MapBufferRange(GL_COPY_READ_BUFFER, 0, m_BufferDescription.SizeInBytes, mapFlags);
					}
				});
		}
		else
		{
			return m_BufferStorage.data();
		}
	}

	void DeviceBufferOpenGL::Unmap()
	{
	}

	void DeviceBufferOpenGL::FlushRange(BufferRange range)
	{
	}

	uint32_t DeviceBufferOpenGL::GetHandle() const
	{ return m_BufferHandle; }

	void DeviceBufferOpenGL::MarkDirty()
	{
	}
}	 // namespace Nexus::Graphics
