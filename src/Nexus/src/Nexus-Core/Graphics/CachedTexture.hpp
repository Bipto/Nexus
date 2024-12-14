#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	class CachedTexture
	{
	  public:
		CachedTexture(PixelFormat format, uint32_t width, uint32_t height) : m_PixelFormat(format), m_Width(width), m_Height(height)
		{
		}

		void AddMipData(uint32_t level, const std::vector<unsigned char> &data)
		{
			/* std::vector<unsigned char> pixels(data.size());
			memcpy(pixels.data(), data.data(), pixels.size());
			m_Pixels[level] = pixels; */
		}

		void Cache(const std::string &path)
		{
			YAML::Node container;
			container["Hash"]	= m_Hash;
			container["Width"]	= m_Width;
			container["Height"] = m_Height;
			container["Pixels"] = m_Pixels;

			YAML::Emitter out;
			out << container;

			FileSystem::WriteFileAbsolute(path, out.c_str());
		}

	  private:
		std::map<uint32_t, std::vector<unsigned char>> m_Pixels		 = {};
		PixelFormat									   m_PixelFormat = {};
		uint32_t									   m_Width		 = 0;
		uint32_t									   m_Height		 = 0;
		size_t										   m_Hash		 = 0;
	};
}	 // namespace Nexus::Graphics