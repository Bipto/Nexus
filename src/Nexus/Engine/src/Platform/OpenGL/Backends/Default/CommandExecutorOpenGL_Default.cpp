#include "Platform/OpenGL/CommandExecutorOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceOpenGL  *deviceGL			= (GraphicsDeviceOpenGL *)device;
		GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();

		DeviceBufferOpenGL *src = dynamic_cast<DeviceBufferOpenGL *>(command.BufferCopy.Source);
		DeviceBufferOpenGL *dst = dynamic_cast<DeviceBufferOpenGL *>(command.BufferCopy.Destination);

		if (src && dst)
		{
			GL::ExecuteGLCommands(
				[&](const GladGLContext &context)
				{
					if (context.ARB_direct_state_access || context.EXT_direct_state_access)
					{
						for (const auto &copy : command.BufferCopy.Copies)
						{
							context.CopyNamedBufferSubData(src->GetHandle(), dst->GetHandle(), copy.ReadOffset, copy.WriteOffset, copy.Size);
						}
					}
					else
					{
						context.BindBuffer(GL_COPY_READ_BUFFER, src->GetHandle());
						context.BindBuffer(GL_COPY_WRITE_BUFFER, dst->GetHandle());

						for (const auto &copy : command.BufferCopy.Copies)
						{
							context.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copy.ReadOffset, copy.WriteOffset, copy.Size);
						}

						context.BindBuffer(GL_COPY_READ_BUFFER, 0);
						context.BindBuffer(GL_COPY_WRITE_BUFFER, 0);
					}
				});
		}
	}
}	 // namespace Nexus::Graphics
