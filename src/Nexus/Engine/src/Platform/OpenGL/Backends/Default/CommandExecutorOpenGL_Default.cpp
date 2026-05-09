#include "Platform/OpenGL/CommandExecutorOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	void CommandExecutorOpenGL::ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)
	{
		GraphicsDeviceOpenGL  *deviceGL			= (GraphicsDeviceOpenGL *)device;
		GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();

		// extract the OpenGL handles from the API objects
		const DeviceBufferOpenGL *src = command.BufferCopy.Source.AsDerived<const DeviceBufferOpenGL>();
		const DeviceBufferOpenGL *dst = command.BufferCopy.Destination.AsDerived<const DeviceBufferOpenGL>();

		// check that the buffer handles are valid
		if (src && dst)
		{
			offscreenContext->Execute(
				[&](const GladGLContext &context)
				{
					// use DSA if it is available
					if (context.ARB_direct_state_access || context.EXT_direct_state_access)
					{
						// loop through each copy operation and execute it
						for (const auto &copy : command.BufferCopy.Copies)
						{
							context.CopyNamedBufferSubData(src->GetHandle(), dst->GetHandle(), copy.ReadOffset, copy.WriteOffset, copy.Size);
						}
					}
					// we need to use legacy binding
					else
					{
						// bind the target buffers
						context.BindBuffer(GL_COPY_READ_BUFFER, src->GetHandle());
						context.BindBuffer(GL_COPY_WRITE_BUFFER, dst->GetHandle());

						// loop through the copy operations and execute them
						for (const auto &copy : command.BufferCopy.Copies)
						{
							context.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copy.ReadOffset, copy.WriteOffset, copy.Size);
						}

						// unbind the target buffers
						context.BindBuffer(GL_COPY_READ_BUFFER, 0);
						context.BindBuffer(GL_COPY_WRITE_BUFFER, 0);
					}
				});
		}
	}
}	 // namespace Nexus::Graphics
