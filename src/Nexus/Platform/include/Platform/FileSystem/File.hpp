#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <tl/expected.hpp>

#include "Platform/Platform-Core.hpp"

namespace Nexus::IO
{
	/// @brief An enumeration representing different file modes for opening files
	enum class FileMode
	{
		/// @brief File opened for reading only, must already exist
		ReadText,

		/// @brief File opened for writing only, creates a new file or overwrites an existing file
		WriteText,

		/// @brief File opened for append only, creates a new file if it doesn't exist. New content is added to the end of the file
		AppendText,

		/// @brief File opened for both reading and writing, must already exist
		ReadWriteText,

		/// @brief Create a new file for reading and writing, overwriting any existing file
		WriteNewText,

		/// @brief File is opened for reading and appending, creates a new file if it doesn't exist
		ReadAppendText,

		/// @brief File opened for reading only, must already exist. Reading is done in binary mode
		ReadBinary,

		/// @brief File opened for writing only, creates a new file or overwrites an existing file. Writing is done in binary mode
		WriteBinary,

		/// @brief File opened for append only, creates a new file if it doesn't exist. New content is added to the end of the file. Appending is done
		/// in binary mode
		AppendBinary,

		/// @brief File opened for both reading and writing, must already exist. Reading and writing is done in binary mode
		ReadWriteBinary,

		/// @brief Create a new file for reading and writing, overwriting any existing file. Reading and writing is done in binary mode
		WriteNewBinary,

		/// @brief File is opened for reading and appending, creates a new file if it doesn't exist. Reading and appending is done in binary mode
		ReadAppendBinary
	};

	/// @brief An enumeration representing different origins for seeking within a file
	enum class SeekOrigin
	{
		/// @brief Seeking will be done from the beginning of the file
		Begin,

		/// @brief Seeking will be done from the current read or write position
		Current,

		/// @brief Seeking will be done from the end of the file
		End
	};

	class FileStreamImpl
	{
	  public:
		virtual ~FileStreamImpl()																		 = default;
		virtual tl::expected<std::vector<uint8_t>, std::string> Read(size_t count)						 = 0;
		virtual tl::expected<size_t, std::string>				Write(const uint8_t *data, size_t count) = 0;
		virtual tl::expected<void, std::string>					Seek(int64_t offset, SeekOrigin origin)	 = 0;
		virtual tl::expected<int64_t, std::string>				GetSize()								 = 0;
		virtual tl::expected<int64_t, std::string>				GetCursorPosition()						 = 0;
	};

	class NX_PLATFORM_API FileStream
	{
	  public:
		FileStream(const std::filesystem::path &path, FileMode fileMode);
		virtual ~FileStream() = default;
		tl::expected<std::vector<uint8_t>, std::string> Read(size_t count);
		tl::expected<size_t, std::string>				Write(const uint8_t *data, size_t count);
		tl::expected<void, std::string>					Seek(int64_t offset, SeekOrigin origin);
		tl::expected<int64_t, std::string>				GetSize();
		tl::expected<int64_t, std::string>				GetCursorPosition();

	  private:
		std::unique_ptr<FileStreamImpl> m_Impl = nullptr;
	};

	class NX_PLATFORM_API File
	{
	  public:
		static tl::expected<std::vector<uint8_t>, std::string> ReadAllBytes(const std::filesystem::path &path);
		static tl::expected<std::string, std::string>		   ReadAllText(const std::filesystem::path &path);
		static tl::expected<size_t, std::string>			   WriteAllBytes(const std::filesystem::path &path, const uint8_t *data, size_t size);
		static tl::expected<size_t, std::string>			   WriteAllText(const std::filesystem::path &path, const std::string &text);
	};
}	 // namespace Nexus::IO