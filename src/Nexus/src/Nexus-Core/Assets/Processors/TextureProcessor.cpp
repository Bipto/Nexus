#include "Nexus-Core/Assets/Processors/TextureProcessor.hpp"

#include "stb_image.h"

namespace Nexus::Processors
{
	TextureProcessor::TextureProcessor()
	{
	}

	TextureProcessor::~TextureProcessor()
	{
	}

	GUID TextureProcessor::Process(const std::string &filepath)
	{
		std::string	   outputFile = filepath + ".bin";
		int			   width, height, channels;
		unsigned char *imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

		size_t dataSize = width * height * channels;

		std::ofstream file(outputFile, std::ios::binary);
		file << width << " " << height << " ";
		file.write((const char *)imageData, dataSize);
		file.close();

		return GUID();
	}
}	 // namespace Nexus::Processors