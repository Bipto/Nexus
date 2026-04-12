#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Platform/OpenGL/GL.hpp"
	#include "RHI/Sampler.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class SamplerOpenGL final : public ISampler
	{
	  public:
		SamplerOpenGL(const SamplerDescription &spec, GraphicsDeviceOpenGL *device);
		virtual ~SamplerOpenGL();
		const SamplerDescription &GetSamplerDescription() const final;
		unsigned int			  GetHandle() const;

		void Bind(uint32_t slot) const;

	  private:
		void Setup(bool hasMips, const GladGLContext &context) const;

	  private:
		GraphicsDeviceOpenGL *m_Device = nullptr;
		SamplerDescription	  m_Description;
		unsigned int		  m_Handle = 0;
	};
}	 // namespace Nexus::Graphics

#endif
