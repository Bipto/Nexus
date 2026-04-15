#pragma once

#include <cstdint>

#include "Nexus-Core/nxpch.hpp"
#include "Platform/IWindow.hpp"
#include "RHI/GraphicsDevice.hpp"

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

	/// @brief A method that loads a new texture from a image stored on disk
	/// @param filepath The filepath to load the image from
	/// @return A pointer to a texture
	NX_API Graphics::TextureHandle CreateTexture2D(Ref<Graphics::ICommandQueue> commandQueue,
												   const char				   *filepath,
												   bool							generateMips,
												   bool							srgb = false);

	/// @brief A method that loads a new texture from an image stored on disk
	/// @param filepath The filepath to load the image from
	/// @return A pointer to a texture
	NX_API Graphics::TextureHandle CreateTexture2D(Ref<Graphics::ICommandQueue> commandQueue,
												   const std::string		   &filepath,
												   bool							generateMips,
												   bool							srgb = false);

	NX_API std::pair<Graphics::TextureHandle, Graphics::TextureViewHandle> CreateTexture2DWithView(Ref<Graphics::ICommandQueue> commandQueue,
																								   const char				   *filepath,
																								   bool							generateMips,
																								   bool							srgb = false);

	NX_API std::pair<Graphics::TextureHandle, Graphics::TextureViewHandle> CreateTexture2DWithView(Ref<Graphics::ICommandQueue> commandQueue,
																								   const std::string		   &filepath,
																								   bool							generateMips,
																								   bool							srgb = false);

	NX_API Graphics::FramebufferHandle CreateFramebuffer(Graphics::IGraphicsDevice							 *device,
														 const Graphics::FramebufferTextureCreateDescription &desc);

	NX_API void WriteToTexture(Ref<Graphics::ICommandQueue> commandQueue,
							   Graphics::TextureHandle		texture,
							   uint32_t						mipLevel,
							   uint32_t						x,
							   uint32_t						y,
							   uint32_t						z,
							   uint32_t						width,
							   uint32_t						height,
							   const void				   *data,
							   size_t						size);

	NX_API std::vector<char> ReadFromTexture(Ref<Graphics::ICommandQueue> commandQueue,
											 Graphics::TextureHandle	  texture,
											 uint32_t					  mipLevel,
											 uint32_t					  x,
											 uint32_t					  y,
											 uint32_t					  z,
											 uint32_t					  width,
											 uint32_t					  height);

	NX_API void WriteToBuffer(Ref<Graphics::ICommandQueue> commandQueue,
							  Ref<Graphics::IDeviceBuffer> buffer,
							  const void				  *data,
							  size_t					   offset,
							  size_t					   size);

	NX_API std::vector<char> ReadFromBuffer(Ref<Graphics::ICommandQueue> commandQueue, Ref<Graphics::IDeviceBuffer> buffer, size_t offset);

}	 // namespace Nexus::Utils