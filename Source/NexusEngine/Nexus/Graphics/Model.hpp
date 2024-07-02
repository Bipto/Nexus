#pragma once

#include "Mesh.hpp"
#include "Nexus/nxpch.hpp"

namespace Nexus::Graphics
{
    class Model
    {
    public:
        Model(std::vector<Ref<Mesh>> meshes)
            : m_Meshes(meshes) {}
        Model() {}

        const std::vector<Ref<Mesh>> &GetMeshes() { return m_Meshes; }

    private:
        std::vector<Ref<Mesh>> m_Meshes;
    };
};