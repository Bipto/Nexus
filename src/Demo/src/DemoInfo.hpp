#pragma once

#include <string>

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Demos/Demo.hpp"

struct DemoInfo
{
	std::string Name																		  = {};
	Demos::Demo *(*CreationFunction)(Nexus::Application *,
									 const std::string						   &name,
									 Nexus::ImGuiUtils::ImGuiGraphicsRenderer  *imGuiRenderer,
									 Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue) = nullptr;
};