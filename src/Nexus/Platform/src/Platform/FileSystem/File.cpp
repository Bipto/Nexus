#include "Platform/FileSystem/File.hpp"
#include "Platform/Platform.hpp"

namespace Nexus::IO
{
	FileStream::FileStream(const std::filesystem::path &path, FileMode fileMode)
	{
		m_Impl = std::unique_ptr<FileStreamImpl>(Platform::CreateFileStreamImpl(path, fileMode));
	}

	std::expected<std::vector<uint8_t>, std::string> FileStream::Read(size_t count)
	{
		return m_Impl->Read(count);
	}

	std::expected<size_t, std::string> FileStream::Write(const uint8_t *data, size_t count)
	{
		return m_Impl->Write(data, count);
	}

	std::expected<void, std::string> FileStream::Seek(int64_t offset, SeekOrigin origin)
	{
		return m_Impl->Seek(offset, origin);
	}

	std::expected<int64_t, std::string> FileStream::GetSize()
	{
		return m_Impl->GetSize();
	}

	std::expected<std::vector<uint8_t>, std::string> File::ReadAllBytes(const std::filesystem::path &path)
	{
		IO::FileStream fileStream(path, IO::FileMode::ReadBinary);
		return fileStream.Read(static_cast<size_t>(fileStream.GetSize().value()));
	}

	std::expected<std::string, std::string> File::ReadAllText(const std::filesystem::path &path)
	{
		IO::FileStream fileStream(path, IO::FileMode::ReadText);
		auto		   result = fileStream.Read(static_cast<size_t>(fileStream.GetSize().value()));
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return std::string(result->begin(), result->end());
	}

	std::expected<size_t, std::string> File::WriteAllBytes(const std::filesystem::path &path, const uint8_t *data, size_t size)
	{
		IO::FileStream fileStream(path, IO::FileMode::WriteNewBinary);
		return fileStream.Write(data, size);
	}

	std::expected<size_t, std::string> File::WriteAllText(const std::filesystem::path &path, const std::string &text)
	{
		IO::FileStream fileStream(path, IO::FileMode::WriteNewText);
		return fileStream.Write(reinterpret_cast<const uint8_t *>(text.data()), text.size());
	}
}	 // namespace Nexus::IO