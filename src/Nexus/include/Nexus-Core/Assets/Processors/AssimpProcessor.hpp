#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Mesh.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"

#define MODELS_PROCESSOR_NAME "Models"

namespace Nexus::Processors
{
	struct ModelImportData
	{
		std::vector<Graphics::MeshData> meshes	  = {};
		std::vector<Graphics::Material> materials = {};
	};

	class NX_API AssimpProcessor : public IProcessor
	{
	  public:
		AssimpProcessor() : IProcessor(MODELS_PROCESSOR_NAME)
		{
		}
		ModelImportData		 LoadModel(const std::string &filepath, Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		Ref<Graphics::Model> Import(const std::string &filepath, Graphics::GraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue);
		GUID				 Process(const std::string			 &filepath,
									 Graphics::GraphicsDevice	 *device,
									 Ref<Graphics::ICommandQueue> commandQueue,
									 Project					 *project) final;
		virtual std::any	 Import(const std::string &filepath) final;
	};

	NX_REGISTER_PROCESSOR(AssimpProcessor, Graphics::Model, MODELS_PROCESSOR_NAME, (std::vector<std::string> {".fbx", ".obj", ".dae"}));
}	 // namespace Nexus::Processors