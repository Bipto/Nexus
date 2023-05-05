#include "MeshFactory.h"

#include <vector>

namespace Nexus
{
    Mesh MeshFactory::CreateCube()
    {
        std::vector<float> vertices = 
        {
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  //0      //BACK
            -0.5f,  0.5f, 0.5f, 0.0f, 1.0f,  //1
             0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  //2
             0.5f,  0.5f, 0.5f, 1.0f, 1.0f,  //3

            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  //4        FRONT
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  //5
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  //6
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f   //7

        };

        std::vector<unsigned int> indices = 
        {
            0, 1, 2,
            1, 3, 2 ,

            4, 5, 6,
            5, 7, 6
        };

        /* std::vector<float> vertices = 
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  //0
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  //1
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  //2
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f   //3

             -0.5f, -0.5f, 1.0f, 0.0f, 0.0f   //4


        };

        std::vector<unsigned int> indices = 
        {
            0, 1, 2,
            1, 3, 2,
        }; */

        auto vertexBuffer = m_Device->CreateVertexBuffer(vertices);
        auto indexBuffer = m_Device->CreateIndexBuffer(indices);

        return Mesh(vertexBuffer, indexBuffer);
    }
}

