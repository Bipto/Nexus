#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Nexus
{
    class OrthographicCamera
    {
        public:
            OrthographicCamera(int width, int height)
            {
                m_Projection = glm::ortho<float>(
                    -width / 2,
                    width / 2,
                    -height / 2,
                    height / 2,
                    -1.0f, 1.0f);
            }
            const glm::mat4 GetProjection(){return this->m_Projection;}
        private:
            glm::mat4 m_Projection;
    };
}