#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "RHI/CommandQueue.hpp"
#include "RHI/Texture.hpp"
#include "RHI/TextureView.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
	class NX_API HdriProcessor
	{
	  public:
		HdriProcessor() = delete;
		HdriProcessor(const std::string &filepath, IGraphicsDevice *device, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
		~HdriProcessor() = default;
		TextureHandle	  Generate(uint32_t size);
		TextureViewHandle GenerateView(uint32_t size);
		TextureHandle	  GetLoadedTexture() const;

	  private:
		void GetDirection(uint32_t faceIndex, float &yaw, float &pitch, bool yUp);

	  private:
		IGraphicsDevice *m_Device = nullptr;
		int32_t			 m_Width  = 0;
		int32_t			 m_Height = 0;

		TextureHandle							   m_HdriImage	  = {};
		Graphics::TextureViewHandle				   m_HdriView	  = {};
		Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue = nullptr;
	};
}	 // namespace Nexus::Graphics