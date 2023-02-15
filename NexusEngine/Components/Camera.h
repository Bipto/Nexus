#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Nexus
{
    class OrthographicCamera
    {
        public:
            OrthographicCamera(int width = 1280, int height = 720, glm::vec3 position = {0, 0, 0})
            {
                this->Resize(width, height);
                this->m_Position = position;
            }

            void Resize(int width, int height)
            {
                this->m_Projection = glm::ortho<float>(
                                    -width / 2,
                                    width / 2,
                                    -height / 2,
                                    height / 2,
                                    -1.0f, 1.0f);
            }

            void SetPosition(const glm::vec3& position)
            {
                m_Position = position;
            }

            const glm::vec3& GetPosition()
            {
                return m_Position;
            }

            const glm::mat4 GetWorld()
            {
                return glm::translate(glm::mat4(1.0f), m_Position);
            }

            const glm::mat4 GetProjection(){ return this->m_Projection; }
        private:
            glm::vec3 m_Position;
            glm::mat4 m_Projection;
            glm::mat4 m_World;
    };
}