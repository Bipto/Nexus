#pragma once

#include "Nexus-Core/Graphics/Fence.hpp"
#include "Platform/OpenGL/GL.hpp"
#include "Platform/Timings/Timespan.hpp"

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
		GLenum Wait(TimeSpan timeout);

	  private:
		void CreateFence(bool signalled);
		void DestroyFence();

	  private:
		GraphicsDeviceOpenGL *m_Device		= nullptr;
		FenceDescription	  m_Description = {};
		GLsync				  m_Sync		= {};
	};
}	 // namespace Nexus::Graphics