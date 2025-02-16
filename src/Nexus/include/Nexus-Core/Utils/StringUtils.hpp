#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::StringUtils
{
	NX_API std::vector<std::string> Split(const std::string &str, const std::string &delimiter);
	NX_API std::string Replace(const std::string &input, const std::string &wordToReplace, const std::string &newWord);
	NX_API std::string RemoveCharacter(const std::string &input, char character);
	NX_API std::string RemoveSubString(const std::string &input, const std::string &substring);
}