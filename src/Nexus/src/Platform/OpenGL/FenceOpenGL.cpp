#include "FenceOpenGL.hpp"

#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	FenceOpenGL::FenceOpenGL(const FenceDescription &desc, GraphicsDeviceOpenGL *device) : m_Description(desc), m_Device(device)
	{
		CreateFence(desc.Signalled);
	}

	FenceOpenGL::~FenceOpenGL()
	{
		DestroyFence();
	}

	bool FenceOpenGL::IsSignalled() const
	{
		GLint status;
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.GetSynciv(m_Sync, GL_SYNC_STATUS, sizeof(status), nullptr, &status); });
		return status == GL_SIGNALED;
	}

	const FenceDescription &FenceOpenGL::GetDescription() const
	{
		return m_Description;
	}

	GLsync FenceOpenGL::GetHandle()
	{
		return m_Sync;
	}

	void FenceOpenGL::Reset()
	{
		DestroyFence();
		CreateFence(false);
	}

	GLenum FenceOpenGL::Wait(TimeSpan timeout)
	{
		GLenum result = 0;
		GL::ExecuteGLCommands([&](const GladGLContext &context)
							  { result = context.ClientWaitSync(m_Sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout.GetNanoseconds<uint64_t>()); });
		return result;
	}

	void FenceOpenGL::CreateFence(bool signalled)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_Sync = context.FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

				// wait for the new fence to be signalled
				if (signalled)
				{
					GLenum result = Wait(TimeSpan::FromNanoseconds(0));
					if (result == GL_WAIT_FAILED)
					{
						throw std::runtime_error("Failed to wait for fence");
					}
				}
			});
	}

	void FenceOpenGL::DestroyFence()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteSync(m_Sync); });
	}
}	 // namespace Nexus::Graphics
