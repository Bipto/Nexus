#include "FileSystem.hpp"

#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/nxpch.hpp"

std::string Nexus::FileSystem::ReadFileToStringAbsolute(const std::string &filepath)
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

void Nexus::FileSystem::WriteFileAbsolute(const std::string &filepath, const std::string &text)
{
	std::filesystem::path path = {filepath};
	std::filesystem::path directory = path.parent_path();

	if (!std::filesystem::exists(directory))
	{
		std::filesystem::create_directories(directory);
	}

	std::ofstream ofs(path);
	ofs << text;
	ofs.close();
}

std::string Nexus::FileSystem::ReadFileToString(const std::string &filepath)
{
	std::string fullpath = GetRootDirectory() + filepath;
	return ReadFileToStringAbsolute(fullpath);
}

void Nexus::FileSystem::WriteFile(const std::string &filepath, const std::string &text)
{
	std::string fullpath = GetRootDirectory() + filepath;
	WriteFileAbsolute(fullpath, text);
}

std::string Nexus::FileSystem::GetFilePathAbsolute(const std::string &filepath)
{
	std::string path = GetRootDirectory() + filepath;
	return path;
}

std::string Nexus::FileSystem::GetRootDirectory()
{
	std::string directory;

#if defined(__ANDROID__) || defined(ANDROID)
	directory = "/data/data/org.libsdl.app/";
#endif

	return directory;
}
