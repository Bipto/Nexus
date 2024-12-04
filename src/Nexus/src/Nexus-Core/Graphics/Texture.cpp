#include "Texture.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "yaml-cpp/yaml.h"

namespace Nexus::Graphics
{
	Texture2D::Texture2D(const Texture2DSpecification &spec, GraphicsDevice *device) : m_Specification(spec), m_Device(device)
	{
	}

	Texture2D::~Texture2D()
	{
	}

	std::vector<unsigned char> Texture2D::GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		std::vector<unsigned char> pixels;
		GetData(pixels, level, x, y, width, height);
		return pixels;
	}

	const Texture2DSpecification &Texture2D::GetSpecification()
	{
		return m_Specification;
	}

	uint32_t Texture2D::GetLevels() const
	{
		return m_Specification.MipLevels;
	}

	Cubemap::Cubemap(const CubemapSpecification &spec, GraphicsDevice *device) : m_Specification(spec), m_Device(device)
	{
	}

	Cubemap::~Cubemap()
	{
	}

	std::vector<unsigned char> Cubemap::GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		std::vector<unsigned char> pixels;
		GetData(pixels, face, level, x, y, width, height);
		return pixels;
	}

	const CubemapSpecification &Cubemap::GetSpecification() const
	{
		return m_Specification;
	}

	uint32_t Cubemap::GetLevels() const
	{
		return m_Specification.MipLevels;
	}

}	 // namespace Nexus::Graphics
