#include "Nexus-Core/Assets/Processors/TextureProcessor.hpp"

#include "Nexus-Core/Graphics/Image.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Runtime/Project.hpp"

namespace Nexus::Processors
{
	GUID TextureProcessor::Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project)
	{
		std::vector<Graphics::Image> mips = {};
		Ref<Graphics::Texture>		 texture = device->CreateTexture2D(filepath.c_str(), m_GenerateMips, m_Srgb);

		for (uint32_t arrayLayer = 0; arrayLayer < texture->GetSpecification().ArrayLayers; arrayLayer++)
		{
			for (uint32_t level = 0; level < texture->GetSpecification().MipLevels; level++)
			{
				Point2D<uint32_t> size = Utils::GetMipSize(texture->GetSpecification().Width, texture->GetSpecification().Height, arrayLayer);
				Graphics::Image	  mip  = Graphics::Image::FromTexture(device, texture, arrayLayer, level, 0, 0, 0, size.X, size.Y);

				if (device->GetGraphicsAPI() == Graphics::GraphicsAPI::OpenGL)
				{
					mip.FlipVertically();
				}

				mips.push_back(mip);
			}
		}

		std::filesystem::path path			 = filepath;
		std::string			  assetPath		 = path.stem().string() + std::string(".texture2d");
		std::filesystem::path outputFilePath = project->GetFullAssetsDirectory() + "/" + assetPath;
		std::ofstream file(outputFilePath, std::ios::binary);

		file << "Texture2D ";
		file << (uint32_t)texture->GetSpecification().Format << " ";
		file << mips.size() << " ";

		for (size_t level = 0; level < mips.size(); level++)
		{
			const Graphics::Image& image = mips[level];
			file << image.Width << " " << image.Height << " ";
			file.write((const char*)image.Pixels.data(), image.Pixels.size());
		}

		file.close();

		Assets::AssetRegistry &registry = project->GetAssetRegistry();
		return registry.RegisterAsset(GetName(), assetPath);
	}

	std::any TextureProcessor::Import(const std::string &filepath)
	{
		return nullptr;
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
