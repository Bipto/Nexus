#include "Platform/OpenGL/CommandExecutorOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)
	{
		GraphicsDeviceOpenGL  *deviceGL			= (GraphicsDeviceOpenGL *)device;
		GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();

		Ref<DeviceBufferOpenGL> src = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferCopy.Source);
		Ref<DeviceBufferOpenGL> dst = std::dynamic_pointer_cast<DeviceBufferOpenGL>(command.BufferCopy.Destination);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				context.BindBuffer(GL_COPY_READ_BUFFER, src->GetHandle());
				context.BindBuffer(GL_COPY_WRITE_BUFFER, dst->GetHandle());

				context.CopyBufferSubData(GL_COPY_READ_BUFFER,
										  GL_COPY_WRITE_BUFFER,
										  command.BufferCopy.ReadOffset,
										  command.BufferCopy.WriteOffset,
										  command.BufferCopy.Size);

				context.BindBuffer(GL_COPY_READ_BUFFER, 0);
				context.BindBuffer(GL_COPY_WRITE_BUFFER, 0);
			});
	}
}	 // namespace Nexus::Graphics
