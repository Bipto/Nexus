#pragma once

#include "GraphicsDeviceVk.hpp"
#include "Nexus-Core/Graphics/AccelerationStructure.hpp"

namespace Nexus::Graphics
{
	class AccelerationStructureVk : public IAccelerationStructure
	{
	  public:
		AccelerationStructureVk(const AccelerationStructureDescription &desc, GraphicsDeviceVk *device);
		virtual ~AccelerationStructureVk();

		const AccelerationStructureDescription &GetDescription() const final;
		VkAccelerationStructureKHR				GetHandle() const;
		VkDeviceAddress							GetDeviceAddress() const;

	  private:
		AccelerationStructureDescription m_Description = {};
		GraphicsDeviceVk				*m_Device	   = nullptr;
		VkAccelerationStructureKHR		 m_Handle	   = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics