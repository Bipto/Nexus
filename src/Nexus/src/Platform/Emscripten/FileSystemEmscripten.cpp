#include "Nexus-Core/FileSystem/FileSystemNew.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

namespace Nexus::FileSystemNew
{
	EM_JS(void, setup_filesystem, (), {
		FS.mkdir('persistent');
		FS.mount(IDBFS, {root: '/persistent', autoPersist: true}, '/persistent');
	});

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

	EM_JS(void, write_string_to_file, (const char *path, const char *context), {
		var filepath	= UTF8ToString(path);
		var fileContent = UTF8ToString(context);
		FS.writeFile(filepath, fileContent);

		alert(FS.cwd());
	});

	EM_JS(char *, read_string_from_file, (const char *path), {
		var filepath		 = UTF8ToString(path);
		var content			 = FS.readFile(filepath, {encoding: 'utf8'});
		var lengthBytes		 = lengthBytesUTF8(content) + 1;
		var stringOnWasmHeap = _malloc(lengthBytes);
		stringToUTF8(content, stringOnWasmHeap, lengthBytes);
		return stringOnWasmHeap;
	});

	EM_JS(int, read_file_to_buffer, (const char *path, char *buffer), {
		var filePath	  = UTF8ToString(path);
		var fileContent	  = FS.readFile(filePath);
		var contentLength = fileContent.length;

		if (contentLength > contentLength)
		{
			console.error("Buffer size is too small for the file content");
			return -1;
		}

		HEAPU8.set(fileContent, buffer);
		return contentLength;
	});

	EM_JS(void, write_file_from_buffer, (const char *path, char *buffer, int size), {
		var filepath   = UTF8ToString(path);
		var fileBuffer = new Uint8Array(HEAPU8.subarray(buffer, buffer + size));
		FS.writeFile(filepath, fileBuffer);
	});

	void Initialise()
	{
		// setup_filesystem();
	}

	void WriteStringToFile(const std::string &data, const std::string &filepath)
	{
		// write_string_to_file(filepath.c_str(), data.c_str());
	}

	std::string ReadStringFromFile(const std::string &filepath)
	{
		std::string content = read_string_from_file(filepath.c_str());
		return content;
	}

	void WriteBufferToFile(void *data, size_t size, const std::string &filepath)
	{
		// write_file_from_buffer(filepath.c_str(), (char *)data, size);
	}

	std::vector<char> ReadBufferFromFile(const std::string &filepath)
	{
		int				  fileSize = get_file_size(filepath.c_str());
		std::vector<char> readBuffer(fileSize);
		int				  bytesRead = read_file_to_buffer(filepath.c_str(), readBuffer.data());
		return readBuffer;
	}

	bool IsWritingFilesSupported()
	{
		return false;
	}

	bool DoesFileExist(const std::string &filepath)
	{
		return std::filesystem::exists(std::filesystem::path(filepath));
	}

	bool DoesDirectoryExist(const std::string &directory)
	{
		return std::filesystem::exists(std::filesystem::path(directory));
	}

}	 // namespace Nexus::FileSystemNew