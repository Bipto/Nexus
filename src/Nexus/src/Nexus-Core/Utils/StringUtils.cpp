#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::StringUtils
{
	std::vector<std::string> Split(const std::string &str, const std::string &delimiter)
	{
		std::vector<std::string> tokens;
		std::string::size_type	 start = 0;
		std::string::size_type	 end   = 0;

		while ((end = str.find(delimiter, start)) != std::string::npos)
		{
			tokens.push_back(str.substr(start, end - start));
			start = end + delimiter.length();
		}
		tokens.push_back(str.substr(start));

		return tokens;
	}

	std::string Replace(const std::string &input, const std::string &wordToReplace, const std::string &newWord)
	{
		std::string text = input;

		size_t pos = text.find(wordToReplace);
		while (pos != std::string::npos)
		{
			text.replace(pos, wordToReplace.length(), newWord);
			pos = text.find(wordToReplace, pos + newWord.length());
		}

		return text;
	}

	std::string RemoveCharacter(const std::string &input, char character)
	{
		std::string text = input;
		text.erase(std::remove(text.begin(), text.end(), character), text.end());
		return text;
	}

	std::string RemoveSubString(const std::string &input, const std::string &substring)
	{
		std::string result = input;
		size_t		pos	   = result.find(substring);
		while (pos != std::string::npos)
		{
			result.erase(pos, substring.length());
			pos = result.find(substring, pos);
		}

		return result;
	}

}	 // namespace Nexus::StringUtils
