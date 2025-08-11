#if defined(NX_PLATFORM_OPENGL)

	#include "SamplerOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	SamplerOpenGL::SamplerOpenGL(const SamplerDescription &spec, GraphicsDeviceOpenGL *device) : m_Device(device), m_Description(spec)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				glCall(context.GenSamplers(1, &m_Handle));

				if (context.KHR_debug)
				{
					// the sampler must have been bound at least once to name it
					context.BindSampler(0, m_Handle);
					context.ObjectLabelKHR(GL_SAMPLER, m_Handle, -1, spec.DebugName.c_str());
				}
			});
	}

	SamplerOpenGL::~SamplerOpenGL()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteSamplers(1, &m_Handle); });
	}

	const SamplerDescription &SamplerOpenGL::GetSamplerSpecification()
	{
		return m_Description;
	}

	unsigned int SamplerOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void SamplerOpenGL::Setup(bool hasMips, const GladGLContext &context)
	{
		GLenum min, max;
		GL::GetSamplerFilter(m_Description.SampleFilter, min, max, hasMips);

		// texture sampling options
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, min));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, max));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(m_Description.AddressModeU)));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(m_Description.AddressModeV)));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(m_Description.AddressModeW)));

		// texture anisotropy
		if (m_Description.SampleFilter == SamplerFilter::Anisotropic)
		{
			glCall(context.SamplerParameterf(m_Handle, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, m_Description.MaximumAnisotropy));
		}

		const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(m_Description.TextureBorderColor);

		// border colour
		GLfloat border[] = {color.r, color.g, color.b, color.a};

		// LOD
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_MIN_LOD, m_Description.MinimumLOD));
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_MAX_LOD, m_Description.MaximumLOD));

	#if defined(NX_PLATFORM_GL_DESKTOP)
		glCall(context.SamplerParameterfv(m_Handle, GL_TEXTURE_BORDER_COLOR, border));
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_LOD_BIAS_EXT, m_Description.LODBias));
	#endif

		// texture comparison
		if (m_Description.SamplerComparisonFunction != ComparisonFunction::Never)
		{
			auto comparisonFunction = GL::GetComparisonFunction(m_Description.SamplerComparisonFunction);
			glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
			glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_FUNC, comparisonFunction));
		}
	}

	void SamplerOpenGL::Bind(uint32_t slot, bool hasMips)
	{
		GL::IOffscreenContext *offscreenContext = m_Device->GetOffscreenContext();
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				glCall(context.BindSampler(slot, m_Handle));
				Setup(hasMips, context);
			});
	}
}	 // namespace Nexus::Graphics

#endif
