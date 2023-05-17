#include "FileSystem.h"

#include <fstream>
#include <sstream>

#include "Core/Logging/Log.h"

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