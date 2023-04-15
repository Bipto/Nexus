#pragma once

#include <fstream>
#include <sstream>

namespace Nexus::ShaderUtils
{   
    struct ShaderSources
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    static ShaderSources ParseShader(const std::string& path)
    {
        std::ifstream stream(path);

        enum class ShaderType
        {
            None = -1,
            Vertex = 0,
            Fragment = 1,
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::None;

        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::Vertex;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::Fragment;
            }
            else
            {
                ss[(int)type] << line << "\n";
            }
        }

        ShaderUtils::ShaderSources source;
        source.VertexSource = ss[0].str();
        source.FragmentSource = ss[1].str();
        return source;
    }
}