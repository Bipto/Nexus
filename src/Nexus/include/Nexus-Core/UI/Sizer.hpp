#pragma once

#include "Nexus-Core/UI/Control.hpp"

namespace Nexus::UI
{
	enum class SizeMode
	{
		Pixel,
		Percentage
	};

	struct SizePercentage
	{
		float Percentage = 100.0f;
	};

	struct SizeAbsolute
	{
		uint32_t Pixels = 100;
	};

	using Size = std::variant<SizePercentage, SizeAbsolute>;

	class SizeCalculator
	{
	  public:
		inline static uint32_t GetColumnSizeInPixels(Size size, uint32_t totalSize)
		{
			uint32_t pixels;

			if (SizePercentage *sizePercentage = std::get_if<SizePercentage>(&size))
			{
				pixels = CalculateSizeFromPercentage(*sizePercentage, totalSize);
			}
			else if (SizeAbsolute *sizeAbsolute = std::get_if<SizeAbsolute>(&size))
			{
				pixels = CalculateSizeFromAbsolute(*sizeAbsolute, totalSize);
			}
			else
			{
				throw std::runtime_error("Failed to find a valid column size");
			}
			return pixels;
		}

	  private:
		inline static uint32_t CalculateSizeFromPercentage(SizePercentage size, uint32_t totalSize)
		{
			float percentage = size.Percentage;
			percentage		 = glm::clamp(percentage, 0.0f, 100.0f);
			return totalSize / 100.0f * percentage;
		}

		inline static uint32_t CalculateSizeFromAbsolute(SizeAbsolute size, uint32_t totalSize)
		{
			return size.Pixels;
		}
	};

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