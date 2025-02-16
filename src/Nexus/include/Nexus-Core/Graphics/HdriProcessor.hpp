#pragma once

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class HdriProcessor
	{
	  public:
		NX_API HdriProcessor() = delete;
		NX_API HdriProcessor(const std::string &filepath, GraphicsDevice *device);
		NX_API ~HdriProcessor() = default;
		NX_API Ref<Cubemap> Generate(uint32_t size);

	  private:
		void GetDirection(CubemapFace face, float &yaw, float &pitch);

	  private:
		GraphicsDevice *m_Device   = nullptr;
		int32_t			m_Width	   = 0;
		int32_t			m_Height   = 0;

		Nexus::Ref<Nexus::Graphics::Texture2D> m_HdriImage = nullptr;
	};
}	 // namespace Nexus::Graphics