#pragma once

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
		tl::expected<std::vector<uint8_t>, std::string> Read(size_t count) final;
		tl::expected<size_t, std::string>				Write(const uint8_t *data, size_t count) final;
		tl::expected<void, std::string>					Seek(int64_t offset, SeekOrigin origin) final;
		tl::expected<int64_t, std::string>				GetSize() final;
		tl::expected<int64_t, std::string>				GetCursorPosition() final;

	  private:
		std::unique_ptr<SDL_IOStream, std::function<void(SDL_IOStream *)>> m_File		  = {};
		std::string														   m_ErrorMessage = {};
	};
}	 // namespace Nexus::IO