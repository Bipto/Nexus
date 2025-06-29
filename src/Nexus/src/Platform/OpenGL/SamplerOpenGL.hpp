#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/Sampler.hpp"
	#include "Platform/OpenGL/GL.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class SamplerOpenGL : public Sampler
	{
	  public:
		SamplerOpenGL(const SamplerSpecification &spec, GraphicsDeviceOpenGL *device);
		virtual ~SamplerOpenGL();
		virtual const SamplerSpecification &GetSamplerSpecification() override;
		unsigned int						GetHandle() const;

		void Bind(uint32_t slot, bool hasMips);
		void Setup(bool hasMips, const GladGLContext &context);

	  private:
		GraphicsDeviceOpenGL *m_Device = nullptr;
		SamplerSpecification m_Specification;
		unsigned int		 m_Handle = 0;
	};
}	 // namespace Nexus::Graphics

#endif
