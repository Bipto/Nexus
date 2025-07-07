#if defined(NX_PLATFORM_OPENGL)

	#include "SamplerOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	SamplerOpenGL::SamplerOpenGL(const SamplerSpecification &spec, GraphicsDeviceOpenGL *device) : m_Device(device), m_Specification(spec)
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { glCall(context.GenSamplers(1, &m_Handle)); });
	}

	SamplerOpenGL::~SamplerOpenGL()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteSamplers(1, &m_Handle); });
	}

	const SamplerSpecification &SamplerOpenGL::GetSamplerSpecification()
	{
		return m_Specification;
	}

	unsigned int SamplerOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void SamplerOpenGL::Setup(bool hasMips, const GladGLContext &context)
	{
		GLenum min, max;
		GL::GetSamplerFilter(m_Specification.SampleFilter, min, max, hasMips);

		// texture sampling options
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, min));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, max));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(m_Specification.AddressModeU)));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(m_Specification.AddressModeV)));
		glCall(context.SamplerParameteri(m_Handle, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(m_Specification.AddressModeW)));

		// texture anisotropy
		if (m_Specification.SampleFilter == SamplerFilter::Anisotropic)
		{
			glCall(context.SamplerParameterf(m_Handle, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, m_Specification.MaximumAnisotropy));
		}

		const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(m_Specification.TextureBorderColor);

		// border colour
		GLfloat border[] = {color.r, color.g, color.b, color.a};

		// LOD
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_MIN_LOD, m_Specification.MinimumLOD));
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_MAX_LOD, m_Specification.MaximumLOD));

	#if defined(NX_PLATFORM_GL_DESKTOP)
		glCall(context.SamplerParameterfv(m_Handle, GL_TEXTURE_BORDER_COLOR, border));
		glCall(context.SamplerParameterf(m_Handle, GL_TEXTURE_LOD_BIAS_EXT, m_Specification.LODBias));
	#endif

		// texture comparison
		if (m_Specification.SamplerComparisonFunction != ComparisonFunction::Never)
		{
			auto comparisonFunction = GL::GetComparisonFunction(m_Specification.SamplerComparisonFunction);
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
