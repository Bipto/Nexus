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
		ModelImportData		 LoadModel(const std::string &filepath, Graphics::GraphicsDevice *device);
		Ref<Graphics::Model> Import(const std::string &filepath, Graphics::GraphicsDevice *device);
		GUID				 Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project) final;
	};

	NX_REGISTER_PROCESSOR(AssimpProcessor, MODELS_PROCESSOR_NAME, (std::vector<std::string> {".fbx", ".obj", ".dae"}));
}	 // namespace Nexus::Processors