#include "Nexus-Core/FileSystem/FileSystemNew.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

namespace Nexus::FileSystemNew
{
	void WriteBufferToFile(void *data, size_t size, const std::string &filepath, FileMode mode)
	{
		EM_JS(void, write_file_from_buffer, (const char *path, char *buffer, int size), {
			var filepath   = UTF8ToString(path);
			var fileBuffer = new Uint8Array(HEAPU8.subarray(buffer, buffer + size));
			FS.writeFile(filepath, fileBuffer);
		});

		write_file_from_buffer(filepath.c_str(), data, size);
	}

	EM_JS(int, get_file_size, (const char *path), {
		var filepath = UTF8ToString(path);
		try
		{
			var fileContent = FS.readFile(filepath);
			return fileContent.length;
		}
		catch (e)
		{
			console.error("File not found: " + filepath);
			return -1;
		}
	});

	std::vector<char> ReadBufferFromFile(const std::string &filepath, FileMode mode)
	{
		EM_JS(int, read_file_to_buffer, (const char *path, char *buffer, int size), {
			var filePath	  = UTF8ToString(path);
			var fileContent	  = FS.readFile(filePath);
			var contentLength = fileContent.length;

			if (contentLength > size)
			{
				console.error("Buffer size is too small for the file content");
				return -1;
			}

			HEAPU8.set(fileContent, buffer);
			return contentLength;
		});

		int				  fileSize = get_file_size(filepath.c_str());
		std::vector<char> readBuffer(fileSize);
		int				  bytesRead = read_file_to_buffer(filepath.c_str(), readBuffer.data(), readBuffer.size());
		return readBuffer;
	}
}	 // namespace Nexus::FileSystemNew