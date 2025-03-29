#pragma once

#pragma once

#include "Nexus-Core/UI/Sizer.hpp"

namespace Nexus::UI
{
	struct GridDimension
	{
		Size Width	= SizePercentage {100.0f};
		Size Height = SizePercentage {100.0f};
	};

	struct GridLayout
	{
		uint32_t				   Rows		 = 1;
		uint32_t				   Columns	 = 1;
		std::vector<GridDimension> GridSizes = {GridDimension {}};
	};

	struct GridItemBounds
	{
		uint32_t x		= 0;
		uint32_t y		= 0;
		uint32_t width	= 0;
		uint32_t height = 0;
	};

	class GridSizer : public Sizer
	{
	  public:
		GridSizer() : Sizer()
		{
		}

		virtual ~GridSizer()
		{
		}

		inline void SetLayout(uint32_t rows, uint32_t columns)
		{
			m_Rows	  = rows;
			m_Columns = columns;
		}

	  private:
		void CalculateLayout() final
		{
			uint32_t x			   = 0;
			uint32_t y			   = 0;
			uint32_t controlWidth  = (uint32_t)glm::ceil((double)GetSize().X / (double)m_Rows);
			uint32_t controlHeight = (uint32_t)glm::ceil((double)GetSize().Y / (double)m_Columns);
			size_t	 index		   = 0;

			for (size_t row = 0; row < m_Rows; row++)
			{
				for (size_t column = 0; column < m_Columns; column++)
				{
					Control *control = GetControl(index++);
					if (control)
					{
						control->SetPosition({x, y});
						control->SetSize({controlWidth, controlHeight});
					}

					x += controlWidth;
				}

				y += controlHeight;
				x = 0;
			}
		}

		Control *GetControl(size_t index)
		{
			if (index >= m_Children.size())
			{
				return nullptr;
			}

			return m_Children[index];
		}

	  private:
		uint32_t m_Rows	   = 1;
		uint32_t m_Columns = 1;
	};
}	 // namespace Nexus::UI
