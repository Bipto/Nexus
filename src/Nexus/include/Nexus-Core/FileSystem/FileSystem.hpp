#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileSystem
{
	/// @brief A method to read the contents of a file into a string
	/// @param filepath An absolute path to the file to read
	/// @return A string containing the contents of the file
	NX_API std::string ReadFileToStringAbsolute(const std::string &filepath);

	/// @brief A method to write a string to a file
	/// @param filepath An absolute path to write the file to
	/// @param text A const reference to the text to write
	NX_API void WriteFileAbsolute(const std::string &filepath, const std::string &text);

	/// @brief A method to read a file to a string
	/// @param filepath A filepath relative to the root of the filesystem
	/// @return A string containing the contents of the file
	NX_API std::string ReadFileToString(const std::string &filepath);

	/// @brief A method to write a string to a file
	/// @param filepath A filepath relative to the root of the filesystem
	/// @param text A const reference to the text to write
	NX_API void WriteFile(const std::string &filepath, const std::string &text);

	/// @brief A method to write a buffer to a file
	/// @param filepath A filepath relative to the root of the filesystem
	/// @param data A const pointer to the beginning of the buffer to write
	/// @param size The size of the buffer to write
	NX_API void WriteBuffer(const std::string &filepath, const void *data, size_t size);

	/// @brief A method to write a buffer to an absolute filepath
	/// @param filepath The absolute filepath to write the file at
	/// @param data A const pointer to the beginning of the data to write
	/// @param size The size of the data to write
	NX_API void WriteBufferAbsolute(const std::string &filepath, const void *data, size_t size);

	/// @brief A method to copy a directory from one location to another
	/// @param from The directory to copy from
	/// @param to The directory to copy to
	/// @param recurive Whether the copy should be performed recursively
	NX_API void CopyDirectory(const std::string &from, const std::string &to, bool recurive);

	/// @brief A method to create a new file directory
	/// @param directory The location to create the directory at
	NX_API void CreateFileDirectory(const std::string &directory);

	/// @brief A method that gets the absolute path of the file from a filesystem
	/// relative filepath
	/// @param filepath A filepath relative to the root of the filesystem
	/// @return A string containing the absolte path of the file
	NX_API std::string GetFilePathAbsolute(const std::string &filepath);

	/// @brief A method returning the root directory of the filesystem
	/// @return A string containing the root directory
	NX_API std::string GetRootDirectory();

}	 // namespace Nexus::FileSystem