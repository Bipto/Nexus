#pragma once

#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	class NX_API Sizer : public Control
	{
	  public:
		Sizer() : Control()
		{
		}

		virtual ~Sizer()
		{
		}

	  private:
		void OnResize(uint32_t width, uint32_t height) final
		{
			CalculateLayout();
		}

		virtual void CalculateLayout() = 0;
	};
}	 // namespace Nexus::UI