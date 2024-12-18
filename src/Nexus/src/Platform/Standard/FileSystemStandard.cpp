#include "Nexus-Core/FileSystem/FileSystemNew.hpp"

namespace Nexus::FileSystemNew
{
	void WriteBufferToFile(Buffer<char> buffer, const std::string &filepath, FileMode mode)
	{
		std::filesystem::path path		= {filepath};
		std::filesystem::path directory = path.parent_path();

		if (!directory.empty())
		{
			if (!std::filesystem::exists(directory))
			{
				std::filesystem::create_directories(directory);
			}
		}

		std::ios_base::openmode openMode = std::ofstream::out;

		if (mode == FileMode::Binary)
		{
			openMode |= std::ofstream::binary;
		}

		std::ofstream out(filepath, openMode);
		out.write(buffer.Data, buffer.GetSizeInBytes());
		out.close();
	}

	Buffer<char> ReadBufferFromFile(const std::string &filepath, FileMode mode)
	{
		if (!std::filesystem::exists(filepath))
		{
			std::string message = "File " + filepath + " does not exist!";
			NX_ERROR(message);
			return {};
		}

		std::ios_base::openmode openMode = std::ifstream::in;

		if (mode == FileMode::Binary)
		{
			openMode |= std::ifstream::binary;
		}

		std::ifstream in(filepath, openMode);

		in.seekg(0, std::ios::end);
		std::streamsize size = in.tellg();
		in.seekg(0, std::ios::beg);

		Buffer<char> buffer;
		buffer.Count = size;
		buffer.Data	 = new char[buffer.Count];
		in.read(buffer.Data, buffer.Count);

		in.close();
	}
}	 // namespace Nexus::FileSystemNew