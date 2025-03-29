#pragma once

#include "Nexus-Core/UI/Sizer.hpp"

namespace Nexus::UI
{
	class VerticalSizer : public Sizer
	{
	  public:
		VerticalSizer() : Sizer()
		{
		}

		virtual ~VerticalSizer()
		{
		}

		inline void SetRows(const std::vector<Size> &rows)
		{
			m_Rows = rows;
		}

	  private:
		void CalculateLayout() final
		{
			Nexus::Point2D<uint32_t> position = {0, 0};

			for (size_t i = 0; i < m_Children.size(); i++)
			{
				Nexus::Point2D<uint32_t> controlPos = position;
				Control					*control	= m_Children[i];
				std::optional<Size>		 rowSize	= GetRowSize(i);

				control->SetPosition(controlPos);

				if (rowSize.has_value())
				{
					position.Y += SizeCalculator::GetColumnSizeInPixels(rowSize.value(), GetSize().Y);
				}
				else
				{
					position.Y += control->GetSize().X;
				}
				control->SetSize({GetSize().X, position.Y - controlPos.Y});
			}
		}

		std::optional<Size> GetRowSize(size_t index)
		{
			if (index >= m_Rows.size())
			{
				return {};
			}

			return m_Rows[index];
		}

	  private:
		std::vector<Size> m_Rows;
	};
}	 // namespace Nexus::UI