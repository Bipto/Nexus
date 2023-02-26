#pragma once

#include "Core/nxpch.h"
#include "../Runtime/Project.h"

namespace Nexus
{
    class Serializer
    {
        public:
            virtual std::string Serialize(Nexus::Project* project) = 0;
            virtual Nexus::Project* Deserialize(std::string text) = 0;
    };
}