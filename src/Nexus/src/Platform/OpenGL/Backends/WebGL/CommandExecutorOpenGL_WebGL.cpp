#include "Platform/OpenGL/CommandExecutorOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

namespace Nexus::Graphics
{
	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
	{
		Ref<DeviceBufferOpenGL> src = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferCopy.Source);
		Ref<DeviceBufferOpenGL> dst = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferCopy.Destination);

		src->Bind(GL_COPY_READ_BUFFER);
		dst->Bind(GL_COPY_WRITE_BUFFER);

		std::vector<char> bufferData(command.BufferCopy.Size);
		glGetBufferSubData(GL_COPY_READ_BUFFER, command.BufferCopy.ReadOffset, command.BufferCopy.Size, bufferData.data());
		glBufferSubData(GL_COPY_WRITE_BUFFER, command.BufferCopy.WriteOffset, command.BufferCopy.Size, bufferData.data());

		GL::ClearBufferBinding(GL_COPY_READ_BUFFER);
		GL::ClearBufferBinding(GL_COPY_WRITE_BUFFER);
	}
}	 // namespace Nexus::Graphics
