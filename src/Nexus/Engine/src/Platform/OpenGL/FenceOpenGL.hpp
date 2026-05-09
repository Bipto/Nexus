#pragma once

#include "Platform/OpenGL/GL.hpp"
#include "Platform/Timings/Timespan.hpp"
#include "RHI/Fence.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class FenceOpenGL : public IFence
	{
	  public:
		FenceOpenGL(const FenceDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~FenceOpenGL();

		bool					IsSignalled() const final;
		const FenceDescription &GetDescription() const final;

		GLsync GetHandle() const;
		void   Reset();
		GLenum Wait(uint64_t timeoutNS);

	  private:
		void CreateFence();
		void DestroyFence();

	  private:
		GraphicsDeviceOpenGL *m_Device		= nullptr;
		FenceDescription	  m_Description = {};
		GLsync				  m_Sync		= {};
	};
}	 // namespace Nexus::Graphics