#include "Platform/FileSystem/FileSystem.hpp"
#include "Platform/Logging/Log.hpp"

#include <fstream>
#include <sstream>

std::expected<std::string, std::string> Nexus::FileSystem::ReadFileNew(const std::filesystem::path &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        return std::unexpected("Failed to open file: " + filepath.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.fail() && !file.eof())
    {
        return std::unexpected("Error while reading file: " + filepath.string());
    }

    return buffer.str();
}

std::string Nexus::FileSystem::ReadFileToStringAbsolute(const std::string &filepath)
{
    if (!std::filesystem::exists(filepath))
    {
        std::string message = "File " + filepath + " does not exist!";
        NX_ERROR(message);
        return {};
    }

    std::ifstream stream(filepath);
    std::stringstream buffer;
    buffer << stream.rdbuf();

    return buffer.str();
}

void Nexus::FileSystem::WriteFileAbsolute(const std::string &filepath, const std::string &text)
{
    std::filesystem::path path = {filepath};
    std::filesystem::path directory = path.parent_path();

    if (!directory.empty() && !std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }

    std::fstream file;
    file.open(path, std::ios::out | std::ios::trunc);
    file.write(text.c_str(), text.size());
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

void Nexus::FileSystem::WriteBuffer(const std::string &filepath, const void *data, size_t size)
{
    std::string fullpath = GetRootDirectory() + filepath;
    WriteBufferAbsolute(filepath, data, size);
}

void Nexus::FileSystem::WriteBufferAbsolute(const std::string &filepath, const void *data, size_t size)
{
    std::filesystem::path path = {filepath};
    std::filesystem::path directory = path.parent_path();

    if (!directory.empty())
    {
        if (!std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(directory);
        }
    }

    std::fstream file;
    file.open(path, std::ios::app | std::ios::binary);
    file.write((const char *)data, size);
}

void Nexus::FileSystem::CopyDirectory(const std::string &from, const std::string &to, bool recurive)
{
    std::filesystem::copy_options options = std::filesystem::copy_options::none;

    if (recurive)
    {
        options = std::filesystem::copy_options::recursive;
    }

    std::filesystem::copy(from, to, options);
}

void Nexus::FileSystem::CreateFileDirectory(const std::string &directory)
{
    std::filesystem::create_directories(directory);
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
    directory = "/data/data/com.nexus.demo/";
#endif

    return directory;
}