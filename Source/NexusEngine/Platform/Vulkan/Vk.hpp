#pragma once

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "Core/Graphics/Framebuffer.hpp"

VkFormat GetVkFormatFromNexusFormat(Nexus::Graphics::TextureFormat format);