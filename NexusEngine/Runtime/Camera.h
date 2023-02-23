#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

namespace Nexus
{
    class OrthographicCamera
    {
        public:
            OrthographicCamera(int width = 1280, int height = 720, glm::vec3 position = {0, 0, 0})
            {
                m_Width = width;
                m_Height = height;

                this->Resize(width, height);
                this->m_Position = position;
            }

            void Resize(int width, int height)
            {
                m_Width = width;
                m_Height = height;

                this->m_Projection = glm::ortho<float>(
                                    (-width / 2) / m_Zoom,
                                    (width / 2) / m_Zoom,
                                    (-height / 2) / m_Zoom,
                                    (height / 2) / m_Zoom,
                                    -100.0f, 100.0f);
            }

            void SetPosition(const glm::vec3& position)
            {
                m_Position = position;
            }

            void SetZoom(float zoom)
            {
                m_Zoom = zoom;
                Resize(m_Width, m_Height);
            }

            const glm::vec3& GetPosition()
            {
                return m_Position;
            }

            const float GetZoom()
            {
                return m_Zoom;
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
            float m_Zoom = 1;

            int m_Width;
            int m_Height;
    };
}