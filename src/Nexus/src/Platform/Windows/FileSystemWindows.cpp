#include "Nexus-Core/FileSystem/FileSystemNew.hpp"

namespace Nexus::FileSystemNew
{
	void CreateDirectoriesIfNeeded(const std::string &filepath)
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
	}

	void WriteStringToFile(const std::string &data, const std::string &filepath)
	{
		CreateDirectoriesIfNeeded(filepath);

		std::ios_base::openmode openMode = std::ofstream::out;
		std::ofstream			out(filepath, openMode);
		out.write(data.data(), data.length());
		out.close();
	}

	std::string ReadStringFromFile(const std::string &filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			std::string message = "File " + filepath + " does not exist!";
			NX_ERROR(message);
			return {};
		}

		std::ifstream	  stream(filepath);
		std::stringstream buffer;
		buffer << stream.rdbuf();
		return buffer.str();
	}

	void WriteBufferToFile(void *data, size_t size, const std::string &filepath)
	{
		CreateDirectoriesIfNeeded(filepath);

		std::ios_base::openmode openMode = std::ofstream::out | std::ofstream::binary;
		std::ofstream			out(filepath, openMode);
		out.write((const char *)data, size);
		out.close();
	}

	std::vector<char> ReadBufferFromFile(const std::string &filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			std::string message = "File " + filepath + " does not exist!";
			NX_ERROR(message);
			return {};
		}

		std::ios_base::openmode openMode = std::ifstream::in | std::ifstream::binary;
		std::ifstream			in(filepath, openMode);

		in.seekg(0, std::ios::end);
		std::streamsize size = in.tellg();
		in.seekg(0, std::ios::beg);

		std::vector<char> buffer(size);
		in.read(buffer.data(), buffer.size());
		in.close();

		return buffer;
	}
}	 // namespace Nexus::FileSystemNew