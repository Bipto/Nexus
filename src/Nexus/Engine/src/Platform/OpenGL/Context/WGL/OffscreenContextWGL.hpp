#pragma once

#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
#include "Platform/OpenGL/ContextDescription.hpp"

#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/wgl.h"
#include <RHI/SamplerState.hpp>

namespace Nexus::GL
{
	class OffscreenContextWGL final : public IOffscreenContext
	{
	  public:
		OffscreenContextWGL(const ContextDescription &spec, Graphics::IPhysicalDevice *device);
		virtual ~OffscreenContextWGL();
		bool				 MakeCurrent() final;
		bool				 Validate() final;
		const GladGLContext &GetContext() const final;

		HGLRC GetHGLRC();

		// textures
		std::expected<uint32_t, std::string> CreateTexture(const Graphics::TextureDescription &desc) final;

		// samplers
		std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc) final;
		void								 DestroySampler(uint32_t handle) final;
		void								 BindSampler(uint32_t handle, uint32_t slot) final;

		// fences
		std::expected<GLsync, std::string> CreateFence(const Graphics::FenceDescription &desc) final;
		void							   DestroyFence(GLsync handle) final;
		bool							   IsSignalled(GLsync handle) final;
		GLenum							   WaitForFence(GLsync handle, uint64_t timeout) final;

	  private:
		std::tuple<HPBUFFERARB, HDC, HGLRC> CreatePBufferContext(HDC hdc, const ContextDescription &spec);

	  private:
		HGLRC		m_HGLRC	  = {};
		HPBUFFERARB m_PBuffer = {};
		HDC			m_HDC	  = {};

		OpenGLFunctionContext m_FunctionContext = {};

	  private:
		inline static bool s_GLFunctionsLoaded = false;
	};
}	 // namespace Nexus::GL