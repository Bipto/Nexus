#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Runtime/Project.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Processors
{
	class NX_API IProcessor
	{
	  public:
		IProcessor()																						  = default;
		virtual ~IProcessor()																				  = default;
		virtual GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project) = 0;
	};

	struct ProcessorInfo
	{
		std::function<IProcessor *()> CreationFunction = {};
		std::vector<std::string>	  FileExtensions   = {};
	};

	class ProcessorRegistry
	{
	  public:
		static std::map<std::string, ProcessorInfo> &GetRegistry()
		{
			static std::map<std::string, ProcessorInfo> registry;
			return registry;
		}

		static std::optional<ProcessorInfo> GetProcessorInfo(const std::string &name)
		{
			auto &registry = GetRegistry();
			if (registry.contains(name))
			{
				return registry[name];
			}

			return {};
		}
	};

	extern "C++" inline NX_API std::map<std::string, ProcessorInfo> &GetProcessorRegistry()
	{
		return ProcessorRegistry::GetRegistry();
	}

}	 // namespace Nexus::Processors

#define NX_REGISTER_PROCESSOR(ProcessorType, ProcessorName, Extensions)                                                                              \
	struct ProcessorType##Register                                                                                                                   \
	{                                                                                                                                                \
		ProcessorType##Register()                                                                                                                    \
		{                                                                                                                                            \
			Nexus::Processors::ProcessorInfo info							   = {};                                                                 \
			info.CreationFunction											   = []() { return new ProcessorType(); };                               \
			info.FileExtensions												   = Extensions;                                                         \
			Nexus::Processors::ProcessorRegistry::GetRegistry()[ProcessorName] = info;                                                               \
		}                                                                                                                                            \
	};                                                                                                                                               \
	static ProcessorType##Register instance##ProcessorType##Register;                                                                                \
	\