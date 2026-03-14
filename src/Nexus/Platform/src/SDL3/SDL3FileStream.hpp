#pragma once

#include <expected>
#include <functional>
#include <memory>

#include "Platform/FileSystem/File.hpp"
#include "SDL3Include.hpp"

namespace Nexus::IO
{
	class FileStreamSDL3 final : public FileStreamImpl
	{
	  public:
		FileStreamSDL3(const std::filesystem::path &path, FileMode fileMode);
		virtual ~FileStreamSDL3() = default;
		std::expected<std::vector<std::byte>, std::string> Read(size_t count) final;
		std::expected<size_t, std::string>				   Write(const std::byte *data, size_t count) final;
		std::expected<void, std::string>				   Seek(int64_t offset, SeekOrigin origin) final;
		std::expected<int64_t, std::string>				   GetSize() final;
		std::expected<int64_t, std::string>				   GetCursorPosition() final;

	  private:
		std::unique_ptr<SDL_IOStream, std::function<void(SDL_IOStream *)>> m_File		  = {};
		std::string														   m_ErrorMessage = {};
	};
}	 // namespace Nexus::IO