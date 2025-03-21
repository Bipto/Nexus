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

	GUID TextureProcessor::Process(const std::string &filepath, Graphics::GraphicsDevice *device, Assets::AssetRegistry *registry)
	{
		std::vector<Graphics::Image> mips = {};
		Ref<Graphics::Texture2D> texture = device->CreateTexture2D(filepath.c_str(), m_GenerateMips, m_Srgb);

		for (uint32_t level = 0; level < texture->GetLevels(); level++)
		{
			Graphics::Image mip = texture->GetDataAsImage(level);

			if (device->GetGraphicsAPI() == Graphics::GraphicsAPI::OpenGL)
			{
				mip.FlipVertically();
			}

			mips.push_back(mip);
		}

		std::filesystem::path path			 = filepath;
		std::filesystem::path root			 = path.parent_path();
		std::filesystem::path outputFilePath = root.string() + "/" + path.stem().string() + std::string(".texture2d");
		std::ofstream file(outputFilePath, std::ios::binary);

		file << (uint32_t)texture->GetSpecification().Format << " ";
		file << mips.size() << " ";

		for (size_t level = 0; level < mips.size(); level++)
		{
			const Graphics::Image& image = mips[level];
			file << image.Width << " " << image.Height << " ";
			file.write((const char*)image.Pixels.data(), image.Pixels.size());
		}

		file.close();

		return registry->RegisterAsset(outputFilePath.string());
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
