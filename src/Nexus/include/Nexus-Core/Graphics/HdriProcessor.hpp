#pragma once

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class NX_API HdriProcessor
	{
	  public:
		HdriProcessor() = delete;
		HdriProcessor(const std::string &filepath, GraphicsDevice *device, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
		~HdriProcessor() = default;
		Ref<Texture> Generate(uint32_t size);
		Ref<Texture> GetLoadedTexture() const;

	  private:
		void GetDirection(uint32_t faceIndex, float &yaw, float &pitch, bool yUp);

	  private:
		GraphicsDevice *m_Device = nullptr;
		int32_t			m_Width	 = 0;
		int32_t			m_Height = 0;

		Nexus::Ref<Nexus::Graphics::Texture>	   m_HdriImage	  = nullptr;
		Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue = nullptr;
	};
}	 // namespace Nexus::Graphics