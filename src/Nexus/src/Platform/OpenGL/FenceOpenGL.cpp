#include "FenceOpenGL.hpp"

namespace Nexus::Graphics
{
	FenceOpenGL::FenceOpenGL(const FenceDescription &desc)
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
		glGetSynciv(m_Sync, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
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
		return glClientWaitSync(m_Sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout.GetNanoseconds<uint64_t>());
	}

	void FenceOpenGL::CreateFence(bool signalled)
	{
		m_Sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		// wait for the new fence to be signalled
		if (signalled)
		{
			GLenum result = Wait(TimeSpan::FromNanoseconds(0));
			if (result == GL_WAIT_FAILED)
			{
				throw std::runtime_error("Failed to wait for fence");
			}
		}
	}

	void FenceOpenGL::DestroyFence()
	{
		glDeleteSync(m_Sync);
	}
}	 // namespace Nexus::Graphics
