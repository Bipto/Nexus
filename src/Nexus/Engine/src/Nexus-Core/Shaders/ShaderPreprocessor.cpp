#include "Nexus-Core/Shaders/ShaderPreprocessor.hpp"
#include <Nexus-Core/Resources/IResourceLoader.hpp>

#include <expected>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace Nexus
{
	ShaderPreprocessor::ShaderPreprocessor(IResourceLoader *loader) : m_ResourceLoader(loader)
	{
	}

	std::expected<std::string, std::string> ShaderPreprocessor::PreprocessShader(const std::string				&shader,
																				 const std::vector<std::string> &includeDirectories) const
	{
		std::vector<std::string>		includeStack;
		std::unordered_set<std::string> onceIncluded;
		// Top-level has no filename context
		return PreprocessShader(shader, includeDirectories, includeStack, onceIncluded, "");
	}

	std::expected<std::string, std::string> ShaderPreprocessor::PreprocessShader(const std::string				 &shader,
																				 const std::vector<std::string>	 &includeDirectories,
																				 std::vector<std::string>		 &includeStack,
																				 std::unordered_set<std::string> &onceIncluded,
																				 const std::string				 &currentFile) const
	{
		static const std::regex includeRegex(R"(^\s*#include\s+[<"]([^">]+)[">](.*)$)");
		static const std::regex pragmaOnceRegex(R"(^\s*#pragma\s+once\b)");

		std::stringstream input(shader);
		std::stringstream output;
		std::string		  line;

		// Did the original text end with a newline?
		bool inputEndsWithNewline = !shader.empty() && shader.back() == '\n';

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

				auto data = m_ResourceLoader->LoadBytes(filename);
				if (!data)
					return std::unexpected(data.error());

				std::string loaded(reinterpret_cast<const char *>(data->data()), data->size());
				auto		processed = PreprocessShader(loaded, includeDirectories, includeStack, onceIncluded, filename);

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
			if (shader.find("#pragma once") != std::string::npos)
				onceIncluded.insert(currentFile);
		}

		return output.str();
	}
}	 // namespace Nexus