#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Nexus-Core/Utils/GUID.hpp"

#include "yaml-cpp/yaml.h"

namespace Nexus
{
    struct Entity
    {
    public:
        Entity() = default;
        Entity(GUID id, const std::string &name);
        void SetID(GUID id);
        GUID GetID() const;
        void SetName(const std::string &name);
        const std::string &GetName() const;
        void Serialize(YAML::Emitter &out) const;
        bool Deserialize(YAML::Node &data);

    private:
        GUID m_ID;
        std::string m_Name = "Unnamed Entity";
    };
}