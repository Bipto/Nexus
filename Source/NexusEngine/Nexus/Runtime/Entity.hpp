#pragma once

#include <inttypes.h>
#include <string>

#include "yaml-cpp/yaml.h"

namespace Nexus
{
    struct Entity
    {
    public:
        Entity() = default;
        Entity(uint64_t id, const std::string &name);
        void SetID(uint64_t id);
        uint64_t GetID() const;
        void SetName(const std::string &name);
        const std::string &GetName() const;

    private:
        uint64_t m_ID = 0;
        std::string m_Name = "Unnamed Entity";
    };
}

namespace YAML
{
    template <>
    struct convert<Nexus::Entity>
    {
        static Node encode(const Nexus::Entity &rhs)
        {
            Node node;
            node.push_back(rhs.GetID());
            node.push_back(rhs.GetName());
            return node;
        }

        static bool decode(const Node &node, Nexus::Entity &rhs)
        {
            if (!node.IsSequence())
                return false;

            rhs.SetID(node[0].as<uint64_t>());
            rhs.SetName(node[1].as<std::string>());
            return true;
        }
    };
}