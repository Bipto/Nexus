#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "Core/Logging/Log.h"
#include "Core/Input/Input.h"

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
                m_Width = width;
                m_Height = height;

                //orthographic
                this->m_Projection = glm::ortho<float>(
                                    (-width / 2) / m_Zoom,
                                    (width / 2) / m_Zoom,
                                    (-height / 2) / m_Zoom,
                                    (height / 2) / m_Zoom,
                                    -1000.0f, 1000.0f);

                //perspective
                //m_Projection = glm::perspectiveFov<float>(glm::radians(45.0f), width, height, 0.1f, 1000.0f);
                //m_Projection = glm::perspective<float>(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
            }

            void SetPosition(const glm::vec3& position)
            {
                m_Position = position;
            }
            
            void SetRotation(const glm::vec3& rotation)
            {
                m_Rotation = rotation;
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

            const glm::vec3& GetRotation()
            {
                return m_Rotation;
            }

            const float GetZoom()
            {
                return m_Zoom;
            }

            const glm::mat4 GetView()
            {
                auto rotX = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                auto rotY = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                auto rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

                float yaw = m_Rotation.y - 90.0f;
                float pitch = m_Rotation.x;

                glm::vec3 cameraDirection;
                cameraDirection.x = (float)glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
                cameraDirection.y = (float)glm::sin(glm::radians(-pitch));
                cameraDirection.z = (float)glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

                m_Front = glm::normalize(cameraDirection);

                return glm::lookAt(m_Position, m_Position - m_Front, m_Up)
                 * glm::translate(glm::mat4(1.0f), m_Position);
            }

            const glm::mat4 GetProjection(){ return this->m_Projection; }

            glm::mat4 GetViewProjection()
            {
                return GetView() * m_Projection;
            }
        private:
            glm::vec3 m_Position { 0.0f, 0.0f, -500.0f };
            glm::vec3 m_Rotation { 0.0f, 0.0f, 0.0f };
            glm::vec3 m_Front { 0.0f, 0.0f, -50.0f };
            glm::vec3 m_Up { 0.0f, 1.0f, 0.0f };

            glm::mat4 m_Projection;
            glm::mat4 m_World;
            float m_Zoom = 100;

            int m_Width;
            int m_Height;
    };

    class PerspectiveCamera
    {
        public:
            PerspectiveCamera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up)
                : m_Position(position), m_Front(front), m_Up(up){}

            void Update(int windowWidth, int windowHeight)
            {
                float aspectRatio = (float)windowWidth / (float)windowHeight;
                m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

                if (aspectRatio > 0.0f)
                {
                    m_Projection = glm::perspective(glm::radians(45.0f), aspectRatio, -10000.0f, 10000.0f);
                }

                Move();
                Rotate();
            }

            const glm::mat4& GetProjection() { return m_Projection; }
            const glm::mat4& GetView() { return m_View; }

        private:
            void Move()
            {
                float speed = 2.0f;

                if (Input::IsRightMouseHeld())
                {
                    if (Input::IsKeyHeld(KeyCode::W))
                    {
                        m_Position += speed * m_Front;
                    }

                    if (Input::IsKeyHeld(KeyCode::S))
                    {
                        m_Position -= speed * m_Front;
                    }

                    if (Input::IsKeyHeld(KeyCode::A))
                    {
                        m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * speed;
                    }

                    if (Input::IsKeyHeld(KeyCode::D))
                    {
                        m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * speed;
                    }
                }
            }

            void Rotate()
            {
                m_Yaw += Input::GetMouseMovement().X;
                m_Front += Input::GetMouseMovement().Y;

                m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

                auto cameraDirection = glm::vec3(0.0f);
                cameraDirection.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
                cameraDirection.y = glm::sin(glm::radians(-m_Pitch));
                cameraDirection.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

                m_Front = glm::normalize(cameraDirection);
            }

        private:
            glm::vec3 m_Position;
            glm::vec3 m_Front;
            glm::vec3 m_Up;

            float m_Yaw = -90.0f;
            float m_Pitch = 0.0f;

            glm::mat4 m_Projection;
            glm::mat4 m_View;

            float m_Zoom;
    };
}