#pragma once

#include <memory>

#include "Nexus-Core/nxpch.hpp"

#include "Audio/AudioDevice.hpp"

#include "RHI/GraphicsDevice.hpp"
#include "RHI/IGraphicsAPI.hpp"
#include "RHI/Swapchain.hpp"

namespace Nexus
{
	class NX_API Engine
	{
	  public:
		Engine(Graphics::GraphicsAPI graphicsAPI, Audio::AudioAPI audioAPI);
		Nexus::Graphics::IGraphicsAPI	   *GetGraphicsAPI();
		Nexus::Graphics::IGraphicsDevice   *GetGraphicsDevice();
		Nexus::Graphics::CommandQueueHandle GetGraphicsCommandQueue();

	  private:
		void CreateGraphicsResources();

	  private:
		std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  m_GraphicsAPI			 = {};
		std::unique_ptr<Nexus::Graphics::IGraphicsDevice> m_GraphicsDevice		 = {};
		Nexus::Graphics::CommandQueueHandle				  m_GraphicsCommandQueue = {};
	};
}	 // namespace Nexus