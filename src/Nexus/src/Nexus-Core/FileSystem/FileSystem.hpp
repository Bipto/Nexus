#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileSystem {
/// @brief A method to read the contents of a file into a string
/// @param filepath An absolute path to the file to read
/// @return A string containing the contents of the file
std::string ReadFileToStringAbsolute(const std::string &filepath);

/// @brief A method to write a string to a file
/// @param filepath An absolute path to write the file to
/// @param text A const reference to the text to write
void WriteFileAbsolute(const std::string &filepath, const std::string &text);

/// @brief A method to read a file to a string
/// @param filepath A filepath relative to the root of the filesystem
/// @return A string containing the contents of the file
std::string ReadFileToString(const std::string &filepath);

/// @brief A method to write a string to a file
/// @param filepath A filepath relative to the root of the filesystem
/// @param text A const reference to the text to write
void WriteFile(const std::string &filepath, const std::string &text);

/// @brief A method that gets the absolute path of the file from a filesystem
/// relative filepath
/// @param filepath A filepath relative to the root of the filesystem
/// @return A string containing the absolte path of the file
std::string GetFilePathAbsolute(const std::string &filepath);

/// @brief A method returning the root directory of the filesystem
/// @return A string containing the root directory
std::string GetRootDirectory();
} // namespace Nexus::FileSystem