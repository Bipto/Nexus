#include "TextureProcessor.hpp"
#include "stb_image.h"

namespace Nexus::Processors
{
	GUID TextureProcessor::Process(const std::string &filepath)
	{
		std::string	   outputFile = filepath + ".bin";
		int			   width, height, channels;
		unsigned char *imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

		size_t dataSize = width * height * channels;

		std::ofstream file(outputFile);
		file << width << " " << height << " ";
		file.write((const char *)imageData, dataSize);
		file.close();

		return GUID();
	}
}	 // namespace Nexus::Processors
