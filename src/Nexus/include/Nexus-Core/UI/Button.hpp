#pragma once

#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	class NX_API Button : public Control
	{
	  public:
		Button()
		{
		}

		virtual ~Button()
		{
		}

		void OnMouseEnter() final
		{
			m_DefaultBackgroundColour = m_Style.BackgroundColour;
			m_Style.BackgroundColour  = {1.0f, 1.0f, 1.0f, 1.0f};
		}

		virtual void OnMouseLeave() final
		{
			m_Style.BackgroundColour = m_DefaultBackgroundColour;
		}

	  private:
		glm::vec4 m_DefaultBackgroundColour = {1.0f, 1.0f, 1.0f, 1.0f};
		glm::vec4 m_HoveredColour			= {0.0f, 0.0f, 0.0f, 1.0f};
	};
}	 // namespace Nexus::UI