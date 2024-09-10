#if defined(NX_PLATFORM_OPENGL)

	#include "SamplerOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	SamplerOpenGL::SamplerOpenGL(const SamplerSpecification &spec) : m_Specification(spec)
	{
		glCall(glGenSamplers(1, &m_Handle));
	}

	SamplerOpenGL::~SamplerOpenGL()
	{
		glCall(glDeleteSamplers(1, &m_Handle));
	}

	const SamplerSpecification &SamplerOpenGL::GetSamplerSpecification()
	{
		return m_Specification;
	}

	unsigned int SamplerOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void SamplerOpenGL::Setup(bool hasMips)
	{
		GLenum min, max;
		GL::GetSamplerFilter(m_Specification.SampleFilter, min, max, hasMips);

		// texture sampling options
		glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, min));
		glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, max));
		glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(m_Specification.AddressModeU)));
		glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(m_Specification.AddressModeV)));
		glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(m_Specification.AddressModeW)));

		// texture anisotropy
		if (m_Specification.SampleFilter == SamplerFilter::Anisotropic)
		{
			glCall(glSamplerParameterf(m_Handle, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, m_Specification.MaximumAnisotropy));
		}

		const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(m_Specification.TextureBorderColor);

		// border colour
		GLfloat border[] = {color.r, color.g, color.b, color.a};

		glCall(glSamplerParameterfv(m_Handle, GL_TEXTURE_BORDER_COLOR_EXT, border));

		// LOD
		glCall(glSamplerParameterf(m_Handle, GL_TEXTURE_MIN_LOD, m_Specification.MinimumLOD));
		glCall(glSamplerParameterf(m_Handle, GL_TEXTURE_MAX_LOD, m_Specification.MaximumLOD));

	#if defined(NX_PLATFORM_GL_DESKTOP)
		glCall(glSamplerParameterf(m_Handle, GL_TEXTURE_LOD_BIAS_EXT, m_Specification.LODBias));
	#endif

		// texture comparison
		if (m_Specification.SamplerComparisonFunction != ComparisonFunction::Never)
		{
			auto comparisonFunction = GL::GetComparisonFunction(m_Specification.SamplerComparisonFunction);
			glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
			glCall(glSamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_FUNC, comparisonFunction));
		}
	}

	void SamplerOpenGL::Bind(uint32_t slot, bool hasMips)
	{
		// prevent the case where we only want to view a specific mip giving
		// errors when binding with a texture with multiple levels
		if (m_Specification.MaximumLOD == m_Specification.MinimumLOD)
		{
			hasMips = false;
		}

		glCall(glBindSampler(slot, m_Handle));
		Setup(hasMips);
	}
}	 // namespace Nexus::Graphics

#endif
