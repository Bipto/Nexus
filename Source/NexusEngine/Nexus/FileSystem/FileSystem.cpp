#include "FileSystem.hpp"

#include <fstream>
#include <sstream>

#include "Nexus/Logging/Log.hpp"

std::string Nexus::FileSystem::ReadFileToString(const std::string &filepath)
{
    if (!std::filesystem::exists(filepath))
    {
        std::string message = "File " + filepath + " does not exist!";
        NX_ERROR(message);
        return {};
    }

    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss;
    while (getline(stream, line))
    {
        ss << line;
    }

    return ss.str();
}

void Nexus::FileSystem::WriteFile(const std::string &filepath, const std::string &text)
{
    std::filesystem::path path = {filepath};

    std::ofstream ofs(path);
    ofs << text;
    ofs.close();
}
