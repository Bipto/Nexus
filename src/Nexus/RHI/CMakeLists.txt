include (FetchContent)

set(RHI_SOURCES
	"include/RHI/AccelerationStructure.hpp"
	"include/RHI/Color.hpp"
	"include/RHI/CommandExecutor.hpp"
	"include/RHI/CommandList.hpp"
	"include/RHI/CommandQueue.hpp"
	"include/RHI/ComputeState.hpp"
	"include/RHI/DeviceAddress.hpp"
	"include/RHI/DeviceBuffer.hpp"
	"include/RHI/DeviceError.hpp"
	"include/RHI/Fence.hpp"
	"include/RHI/Framebuffer.hpp"
	"include/RHI/GraphicsAPICreateInfo.hpp"
	"include/RHI/GraphicsCapabilities.hpp"
	"include/RHI/GraphicsDevice.hpp"
	"include/RHI/GraphicsState.hpp"
	"include/RHI/IGraphicsAPI.hpp"
	"include/RHI/IndirectDrawArguments.hpp"
	"include/RHI/IPhysicalDevice.hpp"
	"include/RHI/ISurface.hpp"
	"include/RHI/MeshletState.hpp"
	"include/RHI/PhysicalDeviceFeatures.hpp"
	"include/RHI/Pipeline.hpp"
	"include/RHI/PixelFormat.hpp"
	"include/RHI/PixelFormatProperties.hpp"
	"include/RHI/RayTracingState.hpp"
	"include/RHI/ResourceSet.hpp"
	"include/RHI/Sampler.hpp"
	"include/RHI/SamplerState.hpp"
	"include/RHI/Scissor.hpp"
	"include/RHI/ShaderDataType.hpp"
	"include/RHI/ShaderLanguage.hpp"
	"include/RHI/ShaderModule.hpp"
	"include/RHI/ShaderResources.hpp"
	"include/RHI/ShaderUtils.hpp"
	"include/RHI/Structures.hpp"
	"include/RHI/Swapchain.hpp"
	"include/RHI/SwapchainDescription.hpp"
	"include/RHI/TexelBuffer.hpp"
	"include/RHI/Texture.hpp"
	"include/RHI/TextureView.hpp"
	"include/RHI/TimingQuery.hpp"
	"include/RHI/Types.hpp"
	"include/RHI/VertexBufferLayout.hpp"
	"include/RHI/Viewport.hpp"
	"include/RHI/WindingOrder.hpp"
	"src/RHI/CommandExecutor.cpp"
	"src/RHI/CommandList.cpp"
	"src/RHI/Framebuffer.cpp"
	"src/RHI/PixelFormatProperties.cpp"
	"src/RHI/ResourceSet.cpp"
	"src/RHI/ShaderDataType.cpp"
	"src/RHI/VertexBufferLayout.cpp"
)

add_library(Nexus-RHI ${RHI_SOURCES})

CPMAddPackage(
	NAME glm
	GITHUB_REPOSITORY g-truc/glm
	GIT_TAG 1.0.3
)

target_link_libraries(Nexus-RHI PUBLIC glm::glm)
target_include_directories(Nexus-RHI PUBLIC "include")

target_compile_features(Nexus-RHI PUBLIC cxx_std_23)
set_target_properties(Nexus-RHI PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

target_compile_definitions(Nexus-RHI PUBLIC NX_RHI_EXPORT=1)
target_link_libraries(Nexus-RHI PUBLIC Nexus-Core)

#if (DEFINED BUILD_TESTING)
#	add_subdirectory(tests)
#endif()
#
#if (DEFINED BUILD_BENCHMARKING)
#	add_subdirectory(benchmarks)
#endif()