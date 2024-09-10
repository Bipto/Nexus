#if defined(NX_PLATFORM_OPENGL)

	#include "BufferOpenGL.hpp"

	#include "Nexus-Core/Logging/Log.hpp"

namespace Nexus::Graphics
{
	GLenum GetBufferUsage(BufferUsage usage)
	{
		switch (usage)
		{
			case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			case BufferUsage::Static: return GL_STATIC_DRAW;
			default: throw std::runtime_error("Invalid buffer usage entered");
		}
	}

	VertexBufferOpenGL::VertexBufferOpenGL(const BufferDescription &description, const void *data) : VertexBuffer(description, data)
	{
		GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

		glCall(glGenBuffers(1, &m_Buffer));
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer));
		glCall(glBufferData(GL_ARRAY_BUFFER, description.Size, data, bufferUsage));
	}

	VertexBufferOpenGL::~VertexBufferOpenGL()
	{
		glCall(glDeleteBuffers(1, &m_Buffer));
	}

	void VertexBufferOpenGL::Bind()
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer));
	}

	void VertexBufferOpenGL::Unbind()
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	unsigned int VertexBufferOpenGL::GetHandle()
	{
		return m_Buffer;
	}

	void VertexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Buffer));
		glCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
	}

	IndexBufferOpenGL::IndexBufferOpenGL(const BufferDescription &description, const void *data, IndexBufferFormat format)
		: IndexBuffer(description, data, format)
	{
		GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

		glCall(glGenBuffers(1, &m_Buffer));
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer));
		glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, description.Size, data, bufferUsage));
	}

	IndexBufferOpenGL::~IndexBufferOpenGL()
	{
		glCall(glDeleteBuffers(1, &m_Buffer));
	}

	void IndexBufferOpenGL::Bind()
	{
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer));
	}

	void IndexBufferOpenGL::Unbind()
	{
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	unsigned int IndexBufferOpenGL::GetHandle()
	{
		return m_Buffer;
	}

	void IndexBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer));
		glCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data));
	}

	UniformBufferOpenGL::UniformBufferOpenGL(const BufferDescription &description, const void *data) : UniformBuffer(description, data)
	{
		GLenum bufferUsage = GetBufferUsage(m_Description.Usage);

		glCall(glGenBuffers(1, &m_Buffer));
		glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer));
		glCall(glBufferData(GL_UNIFORM_BUFFER, description.Size, data, bufferUsage));
	}

	UniformBufferOpenGL::~UniformBufferOpenGL()
	{
		glCall(glDeleteBuffers(1, &m_Buffer));
	}

	unsigned int UniformBufferOpenGL::GetHandle()
	{
		return m_Buffer;
	}

	void UniformBufferOpenGL::Unbind()
	{
		glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	void UniformBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer));
		glCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	}
}	 // namespace Nexus::Graphics

#endif