#include "Nexus-Core/Assets/Processors/TextureProcessor.hpp"

#include "Nexus-Core/Graphics/Image.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

namespace Nexus::Processors
{
	TextureProcessor::TextureProcessor()
	{
	}

	TextureProcessor::~TextureProcessor()
	{
	}

	GUID TextureProcessor::Process(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		std::vector<Graphics::Image> mips = {};
		Ref<Graphics::Texture2D> texture = device->CreateTexture2D(filepath.c_str(), m_GenerateMips, m_Srgb);

		for (uint32_t level = 0; level < texture->GetLevels(); level++)
		{
			Graphics::Image mip = texture->GetDataAsImage(level);
			
			/* /* if (device->GetGraphicsAPI() == Graphics::GraphicsAPI::OpenGL)
			{
				mip.FlipVertically();
			}*/

			mips.push_back(mip);
		}

		std::string outputFilePath = filepath + ".bin";
		std::ofstream file(outputFilePath, std::ios::binary);

		for (size_t level = 0; level < 1; level++)
		{
			const Graphics::Image& image = mips[level];
			file << image.Width << " " << image.Height << " ";
			file.write((const char*)image.Pixels.data(), image.Pixels.size());
		}

		file.close();

		return GUID();
	}

	void Nexus::Processors::TextureProcessor::SetSrgb(bool useSrgb)
	{
		m_Srgb = useSrgb;
	}

	void TextureProcessor::SetGenerateMips(bool generateMips)
	{
		m_GenerateMips = generateMips;
	}
}	 // namespace Nexus::Processors
