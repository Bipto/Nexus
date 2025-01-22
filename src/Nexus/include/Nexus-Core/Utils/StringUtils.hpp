#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::StringUtils
{
	std::vector<std::string> Split(const std::string &str, const std::string &delimiter);
	std::string				 Replace(const std::string &input, const std::string &wordToReplace, const std::string &newWord);
	std::string				 RemoveCharacter(const std::string &input, char character);
}