#include "Nexus-Core/Shaders/ShaderPreprocessor.hpp"
#include <Nexus-Core/Resources/IResourceLoader.hpp>

#include <expected>
#include <filesystem>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace
{
	static std::expected<std::string, std::string> ResolveIncludePath(Nexus::IResourceLoader		 *resourceLoader,
																	  const std::string				 &originalPath,
																	  const std::string				 &includePath,
																	  const std::vector<std::string> &includeDirectories)
	{
		std::filesystem::path shaderPath	  = originalPath;
		std::filesystem::path shaderDirectory = shaderPath.parent_path();

		// attempt to load from the current directory of the shader (relative path)
		{
			std::filesystem::path includeLoadPath = shaderDirectory / includePath;
			if (resourceLoader->DoesFileExist(includeLoadPath.string()))
			{
				return includeLoadPath.string();
			}
		}

		// attempt to load from the provided include directories
		for (const std::string &includeDir : includeDirectories)
		{
			// attach the requested include path to the end of the provided include directories and see if we can find a valid file there
			std::filesystem::path  includeLoadPath = includeDir;
			std::filesystem ::path fullIncludePath = includeLoadPath / includePath;

			// check whether a file can be found at that location
			if (resourceLoader->DoesFileExist(fullIncludePath.string()))
			{
				return fullIncludePath.string();
			}
		}

		// otherwise, build an error containing all of the paths that have been searched
		std::stringstream errorMessage = {};
		errorMessage << "Attempting to include shader but could not find a valid file, attempted to search in the following directories, however a "
						"file named: ("
					 << includePath << ") was not found at any of the specified paths";

		// iterate through all include paths and add them to the error string, appending commands to the end if not the last in the list
		for (size_t index = 0; index < includeDirectories.size(); index++)
		{
			const std::string &includeDir = includeDirectories.at(index);
			errorMessage << includeDir;

			if (index != includeDirectories.size())
			{
				errorMessage << "\n";
			}
		}

		// return the built error message
		return std::unexpected(errorMessage.str());
	}
}	 // namespace

namespace Nexus
{
	ShaderPreprocessor::ShaderPreprocessor(IResourceLoader *loader) : m_ResourceLoader(loader)
	{
	}

	std::expected<std::string, std::string> ShaderPreprocessor::PreprocessShader(const std::string				&shaderPath,
																				 const std::string				&shaderText,
																				 const std::vector<std::string> &includeDirectories) const
	{
		std::vector<std::string>		includeStack;
		std::unordered_set<std::string> onceIncluded;

		// Top-level has no filename context, forward the include paths and shader text
		return PreprocessShader(shaderPath, shaderText, includeDirectories, includeStack, onceIncluded, "");
	}

	std::expected<std::string, std::string> ShaderPreprocessor::PreprocessShader(const std::string				 &shaderPath,
																				 const std::string				 &shaderText,
																				 const std::vector<std::string>	 &includeDirectories,
																				 std::vector<std::string>		 &includeStack,
																				 std::unordered_set<std::string> &onceIncluded,
																				 const std::string				 &currentFile) const
	{
		static const std::regex includeRegex(R"(^\s*#include\s+[<"]([^">]+)[">](.*)$)");
		static const std::regex pragmaOnceRegex(R"(^\s*#pragma\s+once\b)");

		std::stringstream input(shaderText);
		std::stringstream output;
		std::string		  line;

		// Did the original text end with a newline?
		bool inputEndsWithNewline = !shaderText.empty() && shaderText.back() == '\n';

		while (true)
		{
			if (!std::getline(input, line))
				break;

			// Is this the last logical line in this shader?
			bool isLastLine = (input.peek() == EOF);

			std::smatch match;

			// Skip #pragma once lines (do not output them)
			if (std::regex_search(line, match, pragmaOnceRegex))
			{
				// Do nothing: no output, no newline
				continue;
			}

			// Handle #include
			if (std::regex_search(line, match, includeRegex))
			{
				std::string filename = match[1].str();

				// #pragma once support: if this file was already included with pragma once, skip it
				if (onceIncluded.contains(filename))
				{
					std::string trailing = match[2].str();

					// Only output trailing text
					output << trailing;

					// Only add newline if trailing text exists AND this is not the last line
					if (!trailing.empty())
					{
						if (!isLastLine)
							output << "\n";
						else if (inputEndsWithNewline)
							output << "\n";
					}

					continue;
				}

				// Cycle detection
				if (std::find(includeStack.begin(), includeStack.end(), filename) != includeStack.end())
					return std::unexpected("Include cycle detected: " + filename);

				includeStack.push_back(filename);

				auto resolvedIncludePath = ResolveIncludePath(m_ResourceLoader, shaderPath, filename, includeDirectories);
				if (!resolvedIncludePath)
				{
					return std::unexpected(resolvedIncludePath.error());
				}

				auto data = m_ResourceLoader->LoadBytes(*resolvedIncludePath);
				if (!data)
					return std::unexpected(data.error());

				std::string loaded(reinterpret_cast<const char *>(data->data()), data->size());
				auto		processed = PreprocessShader(shaderPath, loaded, includeDirectories, includeStack, onceIncluded, filename);

				includeStack.pop_back();

				if (!processed)
					return std::unexpected(processed.error());

				const std::string &includedText = *processed;

				// If the included file itself had #pragma once, it will have
				// inserted its filename into onceIncluded during its processing.
				// We just use the resulting text.

				// Write included file exactly as-is
				output << includedText;

				bool includedEndsWithNewline = !includedText.empty() && includedText.back() == '\n';

				// Append trailing text after the include directive
				std::string trailing = match[2].str();
				output << trailing;

				// Newline rules for include lines:
				// - If not last line:
				//      - if included text ends with newline → don't add another
				//      - else → add one
				// - If last line:
				//      - if original shader ended with newline AND included text
				//        does NOT end with newline → add one
				if (!isLastLine)
				{
					if (!includedEndsWithNewline)
						output << "\n";
				}
				else
				{
					if (inputEndsWithNewline && !includedEndsWithNewline)
						output << "\n";
				}
			}
			else
			{
				// Normal line
				output << line;

				// Newline rules for normal lines:
				// - If not last line: always one newline
				// - If last line and original ended with newline: one newline
				if (!isLastLine || (isLastLine && inputEndsWithNewline))
					output << "\n";
			}
		}

		// If this shader corresponds to a real file and contains #pragma once,
		// mark it as "once included" so future includes of it can be skipped.
		if (!currentFile.empty())
		{
			if (shaderText.find("#pragma once") != std::string::npos)
				onceIncluded.insert(currentFile);
		}

		return output.str();
	}
}	 // namespace Nexus