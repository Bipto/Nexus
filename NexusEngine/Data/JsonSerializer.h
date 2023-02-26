#pragma once

#include "Serializer.h"
#include "../../External/json/single_include/nlohmann/json.hpp"

namespace Nexus
{
    class JsonSerializer : Serializer
    {
        public:
            virtual std::string Serialize(Nexus::Project* project) override
            {
                nlohmann::json j;
                j["name"] = project->GetName();
                return j.dump();
            }

            virtual Nexus::Project* Deserialize(std::string text) override
            {
                return new Nexus::Project("test");
            }
    };
}