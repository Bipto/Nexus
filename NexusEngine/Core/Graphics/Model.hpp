#pragma once

#include "Mesh.hpp"
#include <vector>

namespace Nexus::Graphics
{
    class Model
    {
    public:
        Model(std::vector<Mesh> meshes)
            : m_Meshes(meshes) {}
        Model() {}

        const std::vector<Mesh> &GetMeshes() { return m_Meshes; }

    private:
        std::vector<Mesh> m_Meshes;
    };
};