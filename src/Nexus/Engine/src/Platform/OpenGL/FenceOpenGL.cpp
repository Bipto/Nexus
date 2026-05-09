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
		GLint			status	= -1;
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->Execute([&](const GladGLContext &context) { context.GetSynciv(m_Sync, GL_SYNC_STATUS, sizeof(status), nullptr, &status); });
		return status == GL_SIGNALED;
	}

	const FenceDescription &FenceOpenGL::GetDescription() const
	{
		return m_Description;
	}

	GLsync FenceOpenGL::GetHandle() const
	{
		return m_Sync;
	}

	void FenceOpenGL::Reset()
	{
		DestroyFence();
		CreateFence(false);
	}

	GLenum FenceOpenGL::Wait(uint64_t timeoutNS)
	{
		GLenum			result	= 0;
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->Execute([&](const GladGLContext &context) { result = context.ClientWaitSync(m_Sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeoutNS); });
		return result;
	}

	void FenceOpenGL::CreateFence(bool signalled)
	{
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->Execute(
			[&](const GladGLContext &context)
			{
				m_Sync = context.FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

				// wait for the new fence to be signalled
				if (signalled)
				{
					GLenum result = Wait(0);
					if (result == GL_WAIT_FAILED)
					{
						throw std::runtime_error("Failed to wait for fence");
					}
				}

				if (context.KHR_debug)
				{
					context.ObjectPtrLabelKHR(m_Sync, -1, m_Description.DebugName.c_str());
				}
			});
	}

	void FenceOpenGL::DestroyFence()
	{
		GL::IGLContext *context = m_Device->GetOffscreenContext();
		context->Execute([&](const GladGLContext &context) { context.DeleteSync(m_Sync); });
	}
}	 // namespace Nexus::Graphics
