#pragma once

#include "Mesh.hpp"
#include <vector>

namespace Nexus::Graphics
{
    class Model
    {
    public:
        Model(std::vector<Mesh *> meshes)
            : m_Meshes(meshes) {}
        Model() {}

        virtual ~Model()
        {
            for (int i = 0; i < m_Meshes.size(); i++)
            {
                delete m_Meshes[i];
            }

            m_Meshes.clear();
        }

        const std::vector<Mesh *> &GetMeshes() { return m_Meshes; }

    private:
        std::vector<Mesh *> m_Meshes;
    };
};