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

		inline void SetColumns(const std::vector<Size> &columns)
		{
			m_Columns = columns;
		}

	  private:
		void CalculateLayout() final
		{
			Nexus::Point2D<uint32_t> position = {0, 0};

			for (size_t i = 0; i < m_Children.size(); i++)
			{
				Nexus::Point2D<uint32_t> controlPos = position;
				Control					*control	= m_Children[i];
				std::optional<Size>		 columnSize = GetColumnSize(i);

				control->SetPosition(controlPos);

				if (columnSize.has_value())
				{
					position.X += SizeCalculator::GetColumnSizeInPixels(columnSize.value(), GetSize().X);
				}
				else
				{
					position.X += control->GetSize().X;
				}
				control->SetSize({position.X - controlPos.X, GetSize().Y});
			}
		}

		std::optional<Size> GetColumnSize(size_t index)
		{
			if (index >= m_Columns.size())
			{
				return {};
			}

			return m_Columns[index];
		}

	  private:
		std::vector<Size> m_Columns;
	};
}	 // namespace Nexus::UI
