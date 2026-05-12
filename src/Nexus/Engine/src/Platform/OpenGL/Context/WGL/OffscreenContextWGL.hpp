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

		// texture buffers
		std::expected<uint32_t, std::string> CreateTexelBuffer(const Graphics::TexelBufferDescription &desc) final;
		void								 DestroyTextureBuffer(uint32_t handle) final;
		void								 BindTextureBuffer(uint32_t handle, uint32_t slot) final;

		// samplers
		std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc) final;
		void								 DestroySampler(uint32_t handle) final;
		void								 BindSampler(uint32_t handle, uint32_t slot) final;

		// fences
		std::expected<GLsync, std::string> CreateFence(const Graphics::FenceDescription &desc) final;
		void							   DestroyFence(GLsync handle) final;
		bool							   IsSignalled(GLsync handle) final;
		GLenum							   WaitForFence(GLsync handle, uint64_t timeout) final;

		// shaders
		uint32_t CreateProgram() final;
		void	 AttachShaderModule(uint32_t program, uint32_t shader) final;
		void	 LinkProgram(uint32_t program) final;
		int		 GetProgramiv(uint32_t program, GLenum parameter) final;
		void	 GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) final;
		void	 DetachShader(uint32_t program, uint32_t shader) final;
		void	 UseShader(uint32_t program) final;

		// pipeline state
		void EnableCapability(GLenum capability, bool enable) final;
		void EnableDepthTest(bool enable) final;
		void EnableDepthWrite(bool enable) final;
		void EnableStencilTest(bool enable) final;
		void SetStencilReference(uint32_t reference) final;

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