#include "StringUtils.hpp"

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
}	 // namespace Nexus::StringUtils
