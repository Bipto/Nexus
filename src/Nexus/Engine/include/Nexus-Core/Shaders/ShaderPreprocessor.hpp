#pragma once

#include <expected>
#include <string>
#include <unordered_set>

#include "Nexus-Core/Resources/IResourceLoader.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	class NX_API ShaderPreprocessor
	{
	  public:
		ShaderPreprocessor() = delete;
		ShaderPreprocessor(IResourceLoader *loader);
		std::expected<std::string, std::string> PreprocessShader(const std::string &shader, const std::vector<std::string> &includeDirectories) const;

	  private:
		std::expected<std::string, std::string> PreprocessShader(const std::string				 &shader,
																 const std::vector<std::string>	 &includeDirectories,
																 std::vector<std::string>		 &includeStack,
																 std::unordered_set<std::string> &onceIncluded,
																 const std::string				 &currentFile) const;

	  private:
		IResourceLoader *m_ResourceLoader = nullptr;
	};
}	 // namespace Nexus