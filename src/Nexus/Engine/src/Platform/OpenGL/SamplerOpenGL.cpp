#if defined(NX_PLATFORM_OPENGL)

	#include "SamplerOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	SamplerOpenGL::SamplerOpenGL(const SamplerDescription &spec, GraphicsDeviceOpenGL *device) : m_Device(device), m_Description(spec)
	{
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		m_Handle					   = context->CreateSampler(spec).value();
	}

	SamplerOpenGL::~SamplerOpenGL()
	{
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->DestroySampler(m_Handle);
	}

	const SamplerDescription &SamplerOpenGL::GetSamplerDescription() const
	{
		return m_Description;
	}

	unsigned int SamplerOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void SamplerOpenGL::Bind(uint32_t slot) const
	{
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->BindSampler(m_Handle, slot);
	}
}	 // namespace Nexus::Graphics

#endif
