#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/Utils/FramerateMonitor.hpp"
#include "Nexus-Core/nxpch.hpp"

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
		FirstPersonCamera(Graphics::GraphicsDevice *device, int width = 1280, int height = 720, const glm::vec3 &position = {0, 0, 0})
			: m_Device(device)
		{
			this->Resize(width, height);
			this->m_Position = position;

			m_Device->GetPrimaryWindow()->OnMouseMoved.Bind([&](const MouseMovedEventArgs &event) { Rotate(event.Movement.X, event.Movement.Y); });
			// m_Device->GetPrimaryWindow()->SetRelativeMouseMode(true);

			m_Device->GetPrimaryWindow()->OnMousePressed.Bind(
				[&](const MouseButtonPressedEventArgs &event)
				{
					if (event.Button == MouseButton::Right)
					{
						IWindow *window = m_Device->GetPrimaryWindow();
						if (window)
						{
							window->SetRelativeMouseMode(true);
						}
						m_RotationActive = true;
					}
				});

			m_Device->GetPrimaryWindow()->OnKeyPressed.Bind(
				[&](const KeyPressedEventArgs &event)
				{
					if (event.ScanCode == ScanCode::Escape)
					{
						IWindow *window = m_Device->GetPrimaryWindow();
						if (window)
						{
							window->SetRelativeMouseMode(false);
						}
						m_RotationActive = false;
					}
				});
		}

		void Resize(int width, int height)
		{
			m_Width	 = width;
			m_Height = height;

			RecalculateProjection();
		}

		void Update(int width, int height, TimeSpan time)
		{
			m_Timer.Update();

			m_Width	 = width;
			m_Height = height;

			Move(time);
			m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
			RecalculateProjection();
		}

		void SetPosition(const glm::vec3 &position)
		{
			m_Position = position;
			RecalculateProjection();
		}

		void SetPitch(float pitch)
		{
			m_Pitch = pitch;
		}

		void SetYaw(float yaw)
		{
			m_Yaw = yaw;
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

		const glm::vec3 &GetPosition() const
		{
			return m_Position;
		}

		const glm::vec3 &GetFront() const
		{
			return m_Front;
		}

		const float GetPitch() const
		{
			return m_Pitch;
		}

		const float GetYaw() const
		{
			return m_Yaw;
		}

		const float GetZoom() const
		{
			return m_Zoom;
		}

		const glm::mat4 GetView() const
		{
			return m_View;
		}

		const glm::mat4 GetProjection() const
		{
			return this->m_Projection;
		}

		glm::mat4 GetViewProjection() const
		{
			return GetView() * m_Projection;
		}

	  private:
		inline void Move(TimeSpan time)
		{
			if (!m_RotationActive)
				return;

			float speed = 2.0f * time.GetSeconds();

			std::optional<uint32_t> defaultKeyboard = Platform::GetActiveKeyboardId();

			if (!defaultKeyboard.has_value())
				return;

			if (Input::IsKeyDown(defaultKeyboard.value(), ScanCode::LeftShift) || Input::IsKeyDown(3, ScanCode::RightShift))
			{
				speed *= 2.0f;
			}

			if (Input::IsKeyDown(defaultKeyboard.value(), ScanCode::W))
			{
				m_Position += speed * m_Front;
			}

			if (Input::IsKeyDown(defaultKeyboard.value(), ScanCode::S))
			{
				m_Position -= speed * m_Front;
			}

			if (Input::IsKeyDown(defaultKeyboard.value(), ScanCode::A))
			{
				m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * speed;
			}

			if (Input::IsKeyDown(defaultKeyboard.value(), ScanCode::D))
			{
				m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * speed;
			}

			/* if (Input::IsGamepadConnected())
			{
				m_Position -= speed * m_Front * (Input::GetGamepadAxisLeft(0).Y * (Input::GetGamepadRightTrigger(0) + 1));
				m_Position +=
				glm::normalize(glm::cross(m_Front, m_Up)) * (Input::GetGamepadAxisLeft(0).X / 50.0f * (Input::GetGamepadRightTrigger(0) + 1));
			} */
		}

		inline void Rotate(float x, float y)
		{
			/* if (Input::IsGamepadConnected())
			{
				auto rightStick = Input::GetGamepadAxisRight(0);
				m_Yaw -= rightStick.X * 2.0f;
				m_Pitch -= rightStick.Y * 2.0f;
			} */

			std::optional<uint32_t> defaultMouse = Platform::GetActiveMouseId();

			if (!defaultMouse.has_value())
				return;

			if (m_RotationActive)
			{
				m_Yaw -= x;
				m_Pitch -= y;
			}

			m_Pitch				 = glm::clamp(m_Pitch, 91.0f, 269.0f);
			auto cameraDirection = glm::vec3(0.0f);
			cameraDirection.x	 = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
			cameraDirection.y	 = glm::sin(glm::radians(-m_Pitch));
			cameraDirection.z	 = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

			m_Front = glm::normalize(cameraDirection);
		}

		inline void RecalculateProjection()
		{
			float aspectRatio = (float)m_Width / (float)m_Height;

			if (aspectRatio > 0.0f)
			{
				// orthographic
				if (m_ProjectionType == ProjectionType::Orthographic)
				{
					m_Projection = glm::ortho<float>((-m_Width / 2) / m_Zoom,
													 (m_Width / 2) / m_Zoom,
													 (-m_Height / 2) / m_Zoom,
													 (m_Height / 2) / m_Zoom,
													 0.1f,
													 1000.0f);
				}
				// perspective
				else
				{
					m_Projection = glm::perspectiveFov<float>(glm::radians(m_Zoom), (float)m_Width, (float)m_Height, 0.1f, 1000.0f);
				}
			}
		}

	  private:
		Graphics::GraphicsDevice *m_Device = nullptr;
		glm::vec3 m_Position {0.0f, 0.0f, 5.0f};
		glm::vec3 m_Front {0.0f, 0.0f, 1.0f};
		glm::vec3 m_Up {0.0f, 1.0f, 0.0f};

		glm::mat4 m_Projection;
		glm::mat4 m_View;
		glm::mat4 m_World;
		float	  m_Zoom = 45.0f;

		float m_Pitch = 180.0f;
		float m_Yaw	  = 270.0f;

		int m_Width	 = 0;
		int m_Height = 0;

		ProjectionType			m_ProjectionType = ProjectionType::Perspective;
		Utils::FrameRateMonitor m_Timer			 = {};
		bool					m_RotationActive = false;
	};
}	 // namespace Nexus