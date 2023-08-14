#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/Logging/Log.hpp"
#include "Core/Input/Input.hpp"
#include "Core/Time.hpp"

#include "Core/Graphics/GraphicsDevice.hpp"

#include "imgui.h"

namespace Nexus
{
    enum ProjectionType
    {
        Perspective,
        Orthographic
    };

    class FirstPersonCamera
    {
    public:
        FirstPersonCamera(int width = 1280, int height = 720, const glm::vec3 &position = {0, 0, 0})
        {
            this->Resize(width, height);
            this->m_Position = position;
        }

        void Resize(int width, int height)
        {
            m_Width = width;
            m_Height = height;

            RecalculateProjection();
        }

        void Update(int width, int height, Time time)
        {
            m_Width = width;
            m_Height = height;

            // m_Zoom -= Input::GetMouseScrollMovementY() * 5.0f;
            // m_Zoom = glm::clamp<float>(m_Zoom, 1.0f, 90.0f);

            Move(time);
            m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
            Rotate();
            RecalculateProjection();
        }

        void SetPosition(const glm::vec3 &position)
        {
            m_Position = position;
            RecalculateProjection();
        }

        void SetRotation(const glm::vec3 &rotation)
        {
            m_Rotation = rotation;
            RecalculateProjection();
        }

        void SetFront(const glm::vec3 &front)
        {
            m_Front = front;
            RecalculateProjection();
        }

        void SetZoom(float zoom)
        {
            m_Zoom = zoom;
            Resize(m_Width, m_Height);
            RecalculateProjection();
        }

        const glm::vec3 &GetPosition()
        {
            return m_Position;
        }

        const glm::vec3 &GetRotation()
        {
            return m_Rotation;
        }

        const glm::vec3 &GetFront()
        {
            return m_Front;
        }

        const float GetZoom()
        {
            return m_Zoom;
        }

        const glm::mat4 GetView()
        {
            return m_View;
        }

        const glm::mat4 GetProjection() { return this->m_Projection; }

        glm::mat4 GetViewProjection()
        {
            return GetView() * m_Projection;
        }

    private:
        void Move(Time time)
        {
            float speed = 2.0f * time.GetSeconds();

            if (Input::IsKeyHeld(KeyCode::LeftShift) || Input::IsKeyHeld(KeyCode::RightShift))
            {
                speed *= 2.0f;
            }

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

            if (Input::IsGamepadConnected())
            {
                m_Position -= speed * m_Front * (Input::GetGamepadAxisLeft(0).Y * (Input::GetGamepadRightTrigger(0) + 1));
                m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * (Input::GetGamepadAxisLeft(0).X / 50.0f * (Input::GetGamepadRightTrigger(0) + 1));
            }
        }

        void Rotate()
        {
            if (Input::IsGamepadConnected())
            {
                auto rightStick = Input::GetGamepadAxisRight(0);
                m_Yaw += rightStick.X * 2.0f;
                m_Pitch -= rightStick.Y * 2.0f;
            }

            if (Input::IsMiddleMouseHeld())
            {
                m_Yaw += Input::GetMouseMovement().X;
                m_Pitch -= Input::GetMouseMovement().Y;
            }

            m_Pitch = glm::clamp(m_Pitch, 91.0f, 269.0f);
            auto cameraDirection = glm::vec3(0.0f);
            cameraDirection.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
            cameraDirection.y = glm::sin(glm::radians(-m_Pitch));
            cameraDirection.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

            m_Front = glm::normalize(cameraDirection);
        }

        void
        RecalculateProjection()
        {
            float aspectRatio = (float)m_Width / (float)m_Height;

            if (aspectRatio > 0.0f)
            {
                // orthographic
                if (m_ProjectionType == ProjectionType::Orthographic)
                {
                    m_Projection = glm::ortho<float>(
                        (-m_Width / 2) / m_Zoom,
                        (m_Width / 2) / m_Zoom,
                        (-m_Height / 2) / m_Zoom,
                        (m_Height / 2) / m_Zoom,
                        0.1f, 1000.0f);
                }
                // perspective
                else
                {
                    m_Projection = glm::perspectiveFov<float>(glm::radians(m_Zoom), (float)m_Width, (float)m_Height, 0.1f, 1000.0f);
                }
            }
        }

    private:
        glm::vec3 m_Position{0.0f, 0.0f, 5.0f};
        glm::vec3 m_Rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 m_Front{0.0f, 0.0f, 1.0f};
        glm::vec3 m_Up{0.0f, 1.0f, 0.0f};

        glm::mat4 m_Projection;
        glm::mat4 m_View;
        glm::mat4 m_World;
        float m_Zoom = 45.0f;

        float m_Pitch = 180.0f;
        float m_Yaw = 90.0f;

        int m_Width = 0;
        int m_Height = 0;

        ProjectionType m_ProjectionType = ProjectionType::Perspective;
    };
}