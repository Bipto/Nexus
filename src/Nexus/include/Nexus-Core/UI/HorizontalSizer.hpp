#pragma once

#include "Nexus-Core/UI/Sizer.hpp"

namespace Nexus::UI
{

	class HorizontalSizer : public Sizer
	{
	  public:
		HorizontalSizer() : Sizer()
		{
		}

		virtual ~HorizontalSizer()
		{
		}

	  private:
		void CalculateLayout() final
		{
			Nexus::Point2D<uint32_t> position = {0, 0};
			for (Control *control : m_Children)
			{
				control->SetPosition(position);
				position.X += control->GetSize().X;
			}
		}
	};
}	 // namespace Nexus::UI
