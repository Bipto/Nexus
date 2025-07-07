#pragma once

#include "Nexus-Core/Graphics/Fence.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Platform/OpenGL/GL.hpp"


namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;

	class FenceOpenGL : public Fence
	{
	  public:
		FenceOpenGL(const FenceDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~FenceOpenGL();

		bool					IsSignalled() const final;
		const FenceDescription &GetDescription() const final;

		GLsync GetHandle();
		void   Reset();
		GLenum Wait(TimeSpan timeout);

	  private:
		void CreateFence(bool signalled);
		void DestroyFence();

	  private:
		GraphicsDeviceOpenGL *m_Device		= nullptr;
		FenceDescription m_Description = {};
		GLsync			 m_Sync		   = {};
	};
}	 // namespace Nexus::Graphics