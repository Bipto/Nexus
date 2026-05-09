#include "Platform/OpenGL/Context/Glad2/Glad2OpenGLFunctionContext.hpp"

#include "Platform/OpenGL/GL.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::GL
{
	OpenGLFunctionContext::OpenGLFunctionContext()
	{
	}

	OpenGLFunctionContext::~OpenGLFunctionContext()
	{
		gladLoaderUnloadGLContext(&m_Context);
	}

	bool OpenGLFunctionContext::Load()
	{
		int result = gladLoaderLoadGLContext(&m_Context);
		return result;
	}

	void OpenGLFunctionContext::ExecuteCommands(std::function<void(const GladGLContext &context)> function)
	{
		function(m_Context);
	}

	const GladGLContext &OpenGLFunctionContext::GetContext() const
	{
		return m_Context;
	}

	std::expected<uint32_t, std::string> OpenGLFunctionContext::CreateTexture(const Graphics::TextureDescription &desc)
	{
		return std::expected<uint32_t, std::string>();
	}

	std::expected<uint32_t, std::string> OpenGLFunctionContext::CreateSampler(const Graphics::SamplerDescription &desc)
	{
		uint32_t handle = 0;
		glCall(m_Context.GenSamplers(1, &handle));

		if (m_Context.KHR_debug)
		{
			// the sampler must have been bound at least once to name it
			glCall(m_Context.BindSampler(0, handle));
			glCall(m_Context.ObjectLabelKHR(GL_SAMPLER, handle, -1, desc.DebugName.c_str()));

			bool useMips = desc.MinimumLOD != 0 || desc.MaximumLOD != 0;

			GLenum min, max;
			GL::GetSamplerFilter(desc.SampleFilter, min, max, useMips);

			// texture sampling options
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, min));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, max));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(desc.AddressModeU)));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(desc.AddressModeV)));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(desc.AddressModeW)));

			// texture anisotropy
			if (desc.SampleFilter == Graphics::SamplerFilter::Anisotropic)
			{
				glCall(m_Context.SamplerParameterf(handle, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, desc.MaximumAnisotropy));
			}

			const glm::vec4 color = Nexus::Utils::ColourFromBorderColor(desc.TextureBorderColor);

			// border colour
			GLfloat border[] = {color.r, color.g, color.b, color.a};
			glCall(m_Context.SamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, border));

			// LOD
			glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_MIN_LOD, desc.MinimumLOD));
			glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_MAX_LOD, desc.MaximumLOD));

			if (m_Context.EXT_texture_lod_bias)
			{
				glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_LOD_BIAS_EXT, desc.LODBias));
			}

			// texture comparison
			if (desc.SamplerComparisonFunction != Graphics::ComparisonFunction::Never)
			{
				auto comparisonFunction = GL::GetComparisonFunction(desc.SamplerComparisonFunction);
				glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
				glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_COMPARE_FUNC, comparisonFunction));
			}
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroySampler(uint32_t handle)
	{
		glCall(m_Context.DeleteSamplers(1, &handle));
	}

	void OpenGLFunctionContext::BindSampler(uint32_t handle, uint32_t slot)
	{
		glCall(m_Context.BindSampler(slot, handle));
	}

	std::expected<GLsync, std::string> OpenGLFunctionContext::CreateFence(const Graphics::FenceDescription &desc)
	{
		GLsync handle = m_Context.FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		// wait for the new fence to be signalled
		if (desc.Signalled)
		{
			GLenum result = WaitForFence(handle, 0);
			if (result == GL_WAIT_FAILED)
			{
				throw std::runtime_error("Failed to wait for fence");
			}
		}

		if (m_Context.KHR_debug)
		{
			m_Context.ObjectPtrLabelKHR(handle, -1, desc.DebugName.c_str());
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroyFence(GLsync handle)
	{
		m_Context.DeleteSync(handle);
	}

	bool OpenGLFunctionContext::IsSignalled(GLsync handle)
	{
		GLint status = -1;
		m_Context.GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
		return status == GL_SIGNALED;
	}

	GLenum OpenGLFunctionContext::WaitForFence(GLsync handle, uint64_t timeout)
	{
		return m_Context.ClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
	}

}	 // namespace Nexus::GL
