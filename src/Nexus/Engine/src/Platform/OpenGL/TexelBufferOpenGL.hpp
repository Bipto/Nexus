#pragma once

#include "GraphicsDeviceOpenGL.hpp"
#include "RHI/TexelBuffer.hpp"

namespace Nexus::Graphics
{
	class TexelBufferOpenGL : public ITexelBuffer
	{
	  public:
		TexelBufferOpenGL(const TexelBufferDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~TexelBufferOpenGL();
		const TexelBufferDescription &GetDescription() const final;
		const uint32_t				  GetTexelBufferHandle() const;
		void						  Bind(uint32_t slot) const;

	  private:
		TexelBufferDescription m_Description = {};
		GraphicsDeviceOpenGL  *m_Device		 = nullptr;
		uint32_t			   m_Handle		 = 0;
	};
}	 // namespace Nexus::Graphics