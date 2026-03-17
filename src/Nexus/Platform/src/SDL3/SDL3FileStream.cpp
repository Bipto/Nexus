#include "SDL3FileStream.hpp"

namespace Nexus::IO
{
	static const char *GetFileAccess(FileMode mode)
	{
		switch (mode)
		{
			case FileMode::ReadText: return "r";
			case FileMode::WriteText: return "w";
			case FileMode::AppendText: return "a";
			case FileMode::ReadWriteText: return "r+";
			case FileMode::WriteNewText: return "w+";
			case FileMode::ReadAppendText: return "a+";
			case FileMode::ReadBinary: return "rb";
			case FileMode::WriteBinary: return "wb";
			case FileMode::AppendBinary: return "ab";
			case FileMode::ReadWriteBinary: return "rb+";
			case FileMode::WriteNewBinary: return "wb+";
			case FileMode::ReadAppendBinary: return "ab+";
			default: throw std::runtime_error("Invalid FileMode specified");
		}
	}

	static SDL_IOWhence GetSeekOrigin(SeekOrigin origin)
	{
		switch (origin)
		{
			case SeekOrigin::Begin: return SDL_IO_SEEK_SET;
			case SeekOrigin::Current: return SDL_IO_SEEK_CUR;
			case SeekOrigin::End: return SDL_IO_SEEK_END;
			default: throw std::runtime_error("Invalid SeekOrigin specified");
		}
	}

	FileStreamSDL3::FileStreamSDL3(const std::filesystem::path &path, FileMode fileMode)
	{
		const char *fileAccess = GetFileAccess(fileMode);
		m_File				   = {SDL_IOFromFile(path.string().c_str(), fileAccess), SDL_CloseIO};

		if (!m_File)
		{
			m_ErrorMessage = std::string("Failed to open file: ") + SDL_GetError();
		}
	}

	std::expected<std::vector<std::byte>, std::string> FileStreamSDL3::Read(size_t count)
	{
		std::vector<std::byte> buffer(count);
		size_t				   bytesRead = SDL_ReadIO(m_File.get(), buffer.data(), count);

		if (!m_File)
		{
			return std::unexpected("Failed to open file for reading");
		}

		buffer.resize(bytesRead);
		return buffer;
	}

	std::expected<size_t, std::string> FileStreamSDL3::Write(const std::byte *data, size_t count)
	{
		size_t bytesWritten = SDL_WriteIO(m_File.get(), (void *)data, count);

		if (!m_File)
		{
			return std::unexpected("Failed to open file for writing");
		}

		return bytesWritten;
	}

	std::expected<void, std::string> FileStreamSDL3::Seek(int64_t offset, SeekOrigin origin)
	{
		SDL_IOWhence whence = GetSeekOrigin(origin);

		if (!m_File)
		{
			return std::unexpected("Failed to open file for seeking");
		}

		if (SDL_SeekIO(m_File.get(), offset, whence) == -1)
		{
			return std::unexpected(std::string("Failed to seek in file: ") + SDL_GetError());
		}

		return {};
	}

	std::expected<int64_t, std::string> FileStreamSDL3::GetSize()
	{
		int64_t size = SDL_GetIOSize(m_File.get());
		if (size < 0)
		{
			return std::unexpected(std::string(SDL_GetError()));
		}
		return size;
	}

	std::expected<int64_t, std::string> FileStreamSDL3::GetCursorPosition()
	{
		int64_t position = SDL_TellIO(m_File.get());
		if (position < 0)
		{
			return std::unexpected(std::string(SDL_GetError()));
		}
		return position;
	}
}	 // namespace Nexus::IO