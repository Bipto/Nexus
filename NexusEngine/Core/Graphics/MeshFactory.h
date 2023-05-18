#pragma once

#include "Core/Memory.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Mesh.h"

namespace Nexus
{
    class MeshFactory
    {
        public:
            explicit MeshFactory(Ref<GraphicsDevice> device)
            {
                m_Device = device;
            }

            Mesh CreateCube();
            Mesh CreateSprite();

        private:
            Ref<GraphicsDevice> m_Device = nullptr;
    };
}