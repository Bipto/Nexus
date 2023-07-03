#pragma once

#include <string>

namespace Nexus::FileSystem
{
    /// @brief A method to read the contents of a file into a string
    /// @param filepath The path to the file to read
    /// @return A string containing the contents of the file
    std::string ReadFileToString(const std::string &filepath);
}