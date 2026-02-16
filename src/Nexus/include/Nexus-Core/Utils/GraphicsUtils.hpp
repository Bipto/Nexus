#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
	NX_API Ref<Graphics::ISurface> CreateSurfaceForWindow(Graphics::IGraphicsDevice *graphicsDevice, Nexus::IWindow *window);
	NX_API Ref<Graphics::IShaderModule> CreateShaderModuleFromSpirvFile(Graphics::IGraphicsDevice *graphicsDevice,
																		const std::string		  &filepath,
																		const std::string		  &outputDirectory,
																		Graphics::ShaderStage	   stage);

	NX_API Ref<Graphics::IShaderModule> CreateShaderModuleFromSpirvSource(Graphics::IGraphicsDevice *graphicsDevice,
																		  const std::string			&source,
																		  const std::string			&name,
																		  const std::string			&outputDirectory,
																		  Graphics::ShaderStage		 stage);

	NX_API Ref<Graphics::IShaderModule> GetOrCreateCachedShaderFromSpirvSource(Graphics::IGraphicsDevice *graphicsDevice,
																			   const std::string		 &source,
																			   const std::string		 &name,
																			   const std::string		 &outputDirectory,
																			   Graphics::ShaderStage	  stage);

	NX_API Ref<Graphics::IShaderModule> GetOrCreateCachedShaderFromSpirvFile(Graphics::IGraphicsDevice *graphicsDevice,
																			 const std::string		   &filepath,
																			 const std::string		   &outputDirectory,
																			 Graphics::ShaderStage		stage);
}	 // namespace Nexus::Utils