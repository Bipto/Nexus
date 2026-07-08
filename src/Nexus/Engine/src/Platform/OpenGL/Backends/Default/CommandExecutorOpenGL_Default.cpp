#include "Platform/OpenGL/CommandExecutorOpenGL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
    void CommandExecutorOpenGL::ExecuteCommand(
        const CopyBufferToBufferCommand &command, IGraphicsDevice *device
    )
    {
        GraphicsDeviceOpenGL *deviceGL = (GraphicsDeviceOpenGL *)device;
        GL::IOffscreenContext *offscreenContext = deviceGL->GetOffscreenContext();

        // extract the OpenGL handles from the API objects
        const DeviceBufferOpenGL *src =
            command.BufferCopy.Source.AsDerived<const DeviceBufferOpenGL>();
        const DeviceBufferOpenGL *dst =
            command.BufferCopy.Destination.AsDerived<const DeviceBufferOpenGL>();

        // check that the buffer handles are valid
        if (src && dst)
        {
            for (const auto &copy : command.BufferCopy.Copies)
            {
                offscreenContext->CopyBufferSubData(
                    src->GetHandle(), dst->GetHandle(), copy.ReadOffset,
                    copy.WriteOffset, copy.Size
                );
            }
        }
    }
} // namespace Nexus::Graphics
