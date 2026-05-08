#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace Nexus::UI
{
	struct Position
	{
		uint32_t X = 0;
		uint32_t Y = 0;
	};

	struct Size
	{
		uint32_t Width	= 0;
		uint32_t Height = 0;
	};

	class IControl
	{
	  public:
		virtual ~IControl() = default;
		void AddControl(std::unique_ptr<IControl> control)
		{
			m_Children.push_back(std::move(control));
		}

	  private:
		std::vector<std::unique_ptr<IControl>> m_Children = {};
	};
}	 // namespace Nexus::UI